// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "PCPP_UE4.h"

//#define _DEBUG_LOCK_ON_SYSTEM_

// Throwaway struct representing a "Sort by Distance" filter.
struct FSortByDistance{
	FSortByDistance(const FVector& InSourceLocation) : SourceLocation(InSourceLocation) {}

	FVector SourceLocation;

	bool operator()(const AActor& A, const AActor& B) const {
		float DistanceA = FVector::DistSquared(SourceLocation, A.GetActorLocation());
		float DistanceB = FVector::DistSquared(SourceLocation, B.GetActorLocation());

		return DistanceA < DistanceB;
	}
};

FCollisionQueryParams _GetQueryParams(AActor* Owner) {
	// Ignore the owner.
	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(Owner);

	// Get  any primitives from the owner so that owner colliders don't get swept in.
	TArray<UPrimitiveComponent*> Comps;
	Owner->GetComponents<UPrimitiveComponent>(Comps);
	for (auto I = Comps.CreateIterator(); I; ++I) {
		QueryParams.AddIgnoredComponent(*I);
	}

	return QueryParams;
}

FCollisionResponseParams _GetCollisionResponseParams() {
	FCollisionResponseParams CollisionResponse = FCollisionResponseParams::DefaultResponseParam;
	return CollisionResponse;
}

FCollisionShape _GetTraceShape(float Radius) {
	auto Shape = FCollisionShape();
	Shape.ShapeType = ECollisionShape::Sphere;
	Shape.Sphere.Radius = Radius;
	return Shape;
}

// Sets default values for this component's properties
ULockOnSystem::ULockOnSystem() {
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
	APawn* PawnOwner = nullptr;
	PCPP_UE4::LazyGetOwnerWithInit(this, PawnOwner, [&](APawn* Owner) {
		if (Owner->IsPlayerControlled()) {
			OwnerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
			CameraMode = true;
		}
	});

	if (Config.ReceiveOnly) {
		SetComponentTickEnabled(false);
	}
}

FVector ULockOnSystem::GetNearLocation(){
	// Project from Camera
	if (CameraMode) {
		return OwnerCamera->GetComponentLocation() + OwnerCamera->GetForwardVector()*Config.NearLockDistance;
	}
	// Project from Owner
	return GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector()*Config.NearLockDistance;
}

FVector ULockOnSystem::GetFarLocation(){
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
		_GetTraceShape(Config.LockRadius),
		_GetQueryParams(GetOwner()),
		_GetCollisionResponseParams()
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
	TArray<AActor*> OutActors = {};
	PCPP_UE4::ForEach(SweptTargets, [&](FHitResult& HitResult) {
		if ((HitResult.Actor).IsValid() && HitResult.bBlockingHit) {
			OutActors.Add(HitResult.Actor.Get());
		}
	});

	// Filter out only valid lockable targets.
	TArray<AActor*> LockableActors = {};
	PCPP_UE4::ForEach(OutActors, [&](AActor* Actor) {
		auto LockOnSystem = Cast<ULockOnSystem>(Actor->GetComponentByClass(ULockOnSystem::StaticClass()));
		if (LockOnSystem && LockOnSystem->Lockable) {
			LockableActors.Add(Actor);
		}
	});

	// Sort targets by distance.
	LockableActors.Sort(FSortByDistance(GetOwner()->GetActorLocation()));

	Out = OutActors;

	// Return success so long as atleast one was found.
	if (OutActors.Num() > 0) {
		return true;
	}

	return false;
}

void ULockOnSystem::SetNewLock(AActor * NewLockReference){
	TargetComponent = Cast<ULockOnSystem>(NewLockReference->GetComponentByClass(ULockOnSystem::StaticClass()));
	OnActorLock.Broadcast(NewLockReference);
}



// Called every frame
void ULockOnSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LockedOn) {
		// TargetComponent not defined. Immediately end Lock.
		if (!TargetComponent) {
			EndLockOn();
		}
		else {
			// TargetComponent is no longer lockable, attempt to cycle to a closer lock.
			if (!(TargetComponent->Lockable)) {
				// If cycling fails then just end lock on.
				if (!CycleLock(-1)) {
					EndLockOn();
				}
			}
		}
	}
}

bool ULockOnSystem::BeginLockOn()
{
	// Already Locked, Do Nothing
	if (LockedOn) {
		return false;
	}

	// Attempt to lock, if successful then begin lock on.
	TArray<AActor*> Actors = {};
	auto LockSuccessful = TraceLockableActors(Actors);
	if (LockSuccessful) {
		// Begin by locking closest actor.
		LockedOn = true;
		TargetComponent = Cast<ULockOnSystem>((Actors[0])->GetComponentByClass(ULockOnSystem::StaticClass()));
		OnActorLock.Broadcast(Actors[0]);
	}

	return LockSuccessful;
}

void ULockOnSystem::EndLockOn() {
	// Do nothing if already not locked on.
	if (!LockedOn) {
		return;
	}
	LockedOn = false;
	TargetComponent = nullptr;
	OnActorLock.Broadcast(nullptr);
}

bool ULockOnSystem::CycleLock(int32 IndexOffset) {
	// No reference. Do nothing.
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

	// Otherwise Cycle.
	SetNewLock(Actors[PCPP_UE4::Math::Mod((ReferenceIndex + IndexOffset),Actors.Num())]);
	return true;
}

