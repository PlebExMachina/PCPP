// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeSystem.h"
#include "GameFramework/Character.h"
#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "PCPP_UE4.h"

// Sets default values for this component's properties
UDodgeSystem::UDodgeSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	DodgeStrength = 1000.f;
	LockedTarget = nullptr;
	CharacterOwner = nullptr;
	// ...
}

ACharacter* UDodgeSystem::GetCharacterOwner() {
	return PCPP_UE4::LazyGetOwner(this,CharacterOwner);
}

void UDodgeSystem::DodgeInDirection(FVector DodgeDirection)
{
	auto Owner = GetCharacterOwner();
	if (Owner) {
		auto OwnerInput = DodgeDirection.GetSafeNormal();
		if (OwnerInput.IsNearlyZero()) {
			OwnerInput = UKismetMathLibrary::GetForwardVector(Owner->GetActorRotation());
		}

		// Perform angle correction if locked to a target. 
		// This is to ensure the player doesn't dash "away" when performing left/right dodges.
		if (LockedTarget) {
			// Get Direction Facing Locked Target
			auto RotationTowardsTarget = UKismetMathLibrary::GetDirectionUnitVector(
				GetOwner()->GetActorLocation(),
				LockedTarget->GetActorLocation()
			).Rotation();

			// Get Owner's Dodge Direction
			auto DodgeRotation = DodgeDirection.Rotation();

			// Find how much of a difference there is between the locked target and where the character is dodging.
			auto RotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(RotationTowardsTarget, DodgeRotation);

			// Left Case, Add 45 degrees for Lock Correction
			if ((RotationDifference.Yaw >= 50.f) && (RotationDifference.Yaw <= 105.f)) {
				OwnerInput = OwnerInput.RotateAngleAxis(45.f, FVector(0.f, 0.f, 1.f));
			}

			// Right Case, Subtract 45 degrees for Lock Correction
			if ((RotationDifference.Yaw >= -105.f) && (RotationDifference.Yaw <= -50.f)) {
				OwnerInput = OwnerInput.RotateAngleAxis(-45.f , FVector(0.f, 0.f, 1.f));
			}
		}

		Owner->LaunchCharacter(OwnerInput*DodgeStrength, true, false);
	}
}

void UDodgeSystem::DodgeUsingInput(){
	if (GetCharacterOwner()) {
		auto OwnerInput = GetCharacterOwner()->GetLastMovementInputVector().GetSafeNormal();
		DodgeInDirection(OwnerInput);
	}
}

void UDodgeSystem::BeginPlay() {
	Super::BeginPlay();

	// If lock on system exists hook into it to allow for dodge correction while locked.
	// With current implementation Lazy Evaluation wouldn't help with dynamic binding.
	ULockOnSystem* LockOnSystem = nullptr;
	PCPP_UE4::LazyGetCompWithInit(GetOwner(), LockOnSystem, [&](ULockOnSystem* Comp) {
		(Comp->OnActorLock).AddDynamic(this, &UDodgeSystem::SetLockedTarget);
	});
}

void UDodgeSystem::SetLockedTarget(AActor* NewTarget) {
	LockedTarget = NewTarget;
}