// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

//#define _DEBUG_LOCK_ON_SYSTEM_

// Throwaway struct representing a "Sort by Distance" filter.
struct FSortByDistance
{
	FSortByDistance(const FVector& InSourceLocation) : SourceLocation(InSourceLocation) {}

	FVector SourceLocation;

	bool operator()(const AActor& A, const AActor& B) const
	{
		float DistanceA = FVector::DistSquared(SourceLocation, A.GetActorLocation());
		float DistanceB = FVector::DistSquared(SourceLocation, B.GetActorLocation());

		return DistanceA < DistanceB;
	}
};


// Sets default values for this component's properties
ULockOnSystem::ULockOnSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Config.LockRadius = 250.f;
	Config.FarLockDistance = 1000.f;
	Config.NearLockDistance = 300.f;
	TargetComponent = nullptr;
	Lockable = true;
	CameraMode = false;
	CollisionChannel = ECollisionChannel::ECC_Visibility;
	// ...
}

void ULockOnSystem::BeginPlay() {
	// Get Owner's camera and set it to CameraMode if it's player controlled.
	auto PawnOwner = Cast<APawn>(GetOwner());
	if (PawnOwner) {
		if (PawnOwner->IsPlayerControlled()) {
			OwnerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
			CameraMode = true;
		}
	}

	if (Config.ReceiveOnly) {
		SetComponentTickEnabled(false);
	}
}

FCollisionQueryParams ULockOnSystem::GetQueryParams()
{
	// Ignore the owner.
	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(GetOwner());

	// Get  any primitives from the owner so that owner colliders don't get swept in.
	TArray<UPrimitiveComponent*> Comps;
	GetOwner()->GetComponents<UPrimitiveComponent>(Comps);
	for (auto I = Comps.CreateIterator(); I; ++I) {
		QueryParams.AddIgnoredComponent(*I);
	}

	return QueryParams;
}

FCollisionResponseParams ULockOnSystem::GetCollisionResponseParams()
{
	FCollisionResponseParams CollisionResponse = FCollisionResponseParams::DefaultResponseParam;
	return CollisionResponse;
}

FCollisionShape ULockOnSystem::GetTraceShape()
{
	auto Shape = FCollisionShape(); 
	Shape.ShapeType = ECollisionShape::Sphere; 
	Shape.Sphere.Radius = Config.LockRadius;
	return Shape;
}

FVector ULockOnSystem::GetNearLocation()
{
	// Project from Camera
	if (CameraMode) {
		return OwnerCamera->GetComponentLocation() + OwnerCamera->GetForwardVector()*Config.NearLockDistance;
	}
	// Project from Owner
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector()*Config.NearLockDistance;
}

FVector ULockOnSystem::GetFarLocation()
{
	// Project from Camera
	if (CameraMode) {
		return OwnerCamera->GetComponentLocation() + OwnerCamera->GetForwardVector()*Config.FarLockDistance;
	}
	// Project from Owner
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector()*Config.FarLockDistance;
}

bool ULockOnSystem::TraceLockableActors(TArray<AActor*>& Out)
{
	// Sweep, look for any hittable targets.
	auto SweptTargets = TArray<FHitResult>();
	GetWorld()->SweepMultiByChannel(
		SweptTargets,
		GetNearLocation(),
		GetFarLocation(),
		FQuat(),
		CollisionChannel,
		GetTraceShape(),
		GetQueryParams(),
		GetCollisionResponseParams()
	);


	#ifdef _DEBUG_LOCK_ON_SYSTEM_
	DrawDebugLine(
		GetWorld(),
		GetNearLocation(),
		GetFarLocation(),
		FColor(255, 0, 0),
		true,
		10.f,
		0,
		12.333
	);
	#endif

	// Filter out only hit actors.
	TArray<AActor*> OutActors;
	for (auto I = SweptTargets.CreateIterator(); I; ++I) {
		if ((I->Actor).IsValid() && I->bBlockingHit) {
			OutActors.Add((I->Actor).Get());
		}
	}

	// Filter out only valid lockable targets.
	OutActors = OutActors.FilterByPredicate([&](AActor* const I) {
		auto LockOnSystem = Cast<ULockOnSystem>(I->GetComponentByClass(ULockOnSystem::StaticClass()));
		if (!LockOnSystem) {
			return false;
		}
		return LockOnSystem->Lockable;
	});

	// Sort targets by distance.
	OutActors.Sort(FSortByDistance(GetOwner()->GetActorLocation()));
	Out = OutActors;

	// Return success so long as atleast one was found.
	if (OutActors.Num() > 0) {
		return true;
	}

	return false;
}

void ULockOnSystem::SetNewLock(AActor * NewLockReference)
{
	TargetComponent = Cast<ULockOnSystem>(NewLockReference->GetComponentByClass(ULockOnSystem::StaticClass()));
	OnActorLock.Broadcast(NewLockReference);
}



// Called every frame
void ULockOnSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LockedOn) {
		// TargetComponent not defined. Immediately end Lock.
		if (!TargetComponent) {
			EndLockOn();
		}
		else {
			// TargetComponent is no longer lockable, attempt to cycle to a closer lock.
			if (!(TargetComponent->Lockable)) {
				if (!CycleCloserLock()) {
					EndLockOn();
				}
			}
		}
	}
}

bool ULockOnSystem::BeginLockOn()
{
	// Failure, Already Locked
	if (LockedOn) {
		return false;
	}

	// Attempt to lock, if successful then begin lock on.
	TArray<AActor*> Actors = {};
	auto LockSuccessful = TraceLockableActors(Actors);
	if (LockSuccessful) {
		LockedOn = true;
		TargetComponent = Cast<ULockOnSystem>((Actors[0])->GetComponentByClass(ULockOnSystem::StaticClass()));
		OnActorLock.Broadcast(Actors[0]);
	}

	return LockSuccessful;
}

void ULockOnSystem::EndLockOn() {
	if (!LockedOn) {
		// Do nothing if already not locked.
		return;
	}
	LockedOn = false;
	TargetComponent = nullptr;
	OnActorLock.Broadcast(nullptr);
}

bool ULockOnSystem::CycleCloserLock() {
	// No reference actor. Do Nothing.
	if (!TargetComponent) {
		return false;
	}

	// Perform Trace
	TArray<AActor*> Actors = {};
	TraceLockableActors(Actors);
	
	// If no targets then End Lock.
	if (Actors.Num() == 0) {
		// If search failed (such as on auto re-lock) then end lock and report failure.
		EndLockOn();
		return false;
	}

	int32 ReferenceIndex = Actors.Find(TargetComponent->GetOwner());

	// Active target now missing, default to closest.
	if (ReferenceIndex == INDEX_NONE) {
		SetNewLock(Actors[0]);
		return true;
	}

	// First member case, cycle to furthest.
	if (ReferenceIndex == 0) {
		SetNewLock(Actors.Last());
	}
	// Otherwise Cycle Closer
	else {
		SetNewLock(Actors[abs((ReferenceIndex - 1) % Actors.Num())]);
	}

	return true;
}

bool ULockOnSystem::CycleFurtherLock() {
	// No reference actor. Exit.
	if (!TargetComponent) {
		return false;
	}

	// Perform trace.
	TArray<AActor*> Actors = {};
	TraceLockableActors(Actors);

	// Nothing to lock onto. End Lock On.
	if (Actors.Num() == 0) {
		EndLockOn();
		return false;
	}

	int32 ReferenceIndex = Actors.Find(TargetComponent->GetOwner());

	// Reference not in array, default to closest.
	if (ReferenceIndex == INDEX_NONE) {
		SetNewLock(Actors[0]);
		return true;
	}
	// Otherwise Cycle further.
	SetNewLock(Actors[abs((ReferenceIndex + 1) % Actors.Num())]);

	return true;
}

