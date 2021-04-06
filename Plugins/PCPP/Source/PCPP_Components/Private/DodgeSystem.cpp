// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeSystem.h"
#include "GameFramework/Character.h"
#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (!CharacterOwner) {
		CharacterOwner = Cast<ACharacter>(GetOwner());
	}
	return CharacterOwner;
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
		if (LockedTarget) {
			auto RotationTowardsTarget = UKismetMathLibrary::GetDirectionUnitVector(
				GetOwner()->GetActorLocation(),
				LockedTarget->GetActorLocation()
			).Rotation();
			auto DodgeRotation = DodgeDirection.Rotation();

			auto RotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(RotationTowardsTarget, DodgeRotation);
			if ((RotationDifference.Yaw >= 50.f) && (RotationDifference.Yaw <= 105.f)) {
				// Left Case, Subtract 45 degrees for Lock Correction
				OwnerInput = OwnerInput.RotateAngleAxis(45.f,FVector(0.f,0.f,1.f));
			} else if ((RotationDifference.Yaw >= -105.f) && (RotationDifference.Yaw <= -50.f)) {
				// Right Case, Add 45 for Lock Correction
				OwnerInput = OwnerInput.RotateAngleAxis(-45.f , FVector(0.f, 0.f, 1.f));
			}
		}

		Owner->LaunchCharacter(OwnerInput*DodgeStrength, true, false);
	}
}

void UDodgeSystem::DodgeUsingInput()
{
	auto Owner = GetCharacterOwner();
	if (Owner) {
		auto OwnerInput = Owner->GetLastMovementInputVector().GetSafeNormal();
		DodgeInDirection(OwnerInput);
	}
}

void UDodgeSystem::BeginPlay() {
	Super::BeginPlay();

	// If lock on system exists hook into it to allow for dodge correction while locked.
	// With current implementation Lazy Evaluation wouldn't help with dynamic binding.
	auto LockOnSystem = Cast<ULockOnSystem>(GetOwner()->GetComponentByClass(ULockOnSystem::StaticClass()));
	if (LockOnSystem) {
		(LockOnSystem->OnActorLock).AddDynamic(this, &UDodgeSystem::SetLockedTarget);
	}
}

void UDodgeSystem::SetLockedTarget(AActor* NewTarget) {
	LockedTarget = NewTarget;
}