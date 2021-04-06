// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DDDCharacterMovement.h"
#include "Components/InputComponent.h"
#include "LockOnSystem.h"
#include "GameFramework/Character.h"
#include "DDDDriver.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UDDDDriver : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDDDDriver();

protected:

	// If lock on system detected (optional) will add functionality to prevent camera turning when locked.
	UFUNCTION()
	void TrackLockOn(AActor* ActorLocked);
	bool LockedOn;

	ULockOnSystem* LockOnSystem;

	// Getter that will initialize lock on system if needed. (Lazy Evaluation)
	ULockOnSystem* GetLockOnSystem();

	float PreviousUpTurnInput;
	float PreviousRightTurnInput;
	bool LockCycleAvailable;
	void VerifyLockCycleAvailable();
	void TryCycleLock(float AxisValue);

	UPROPERTY(EditAnywhere)
	float LockInputSensitivity;

	// The Character reference of the owner.
	ACharacter* CharacterOwner;
	ACharacter* GetCharacterOwner();

	// The DDD Component that is the basis of the driver.
	UDDDCharacterMovement* MovementComponent;
	UDDDCharacterMovement* GetMovementComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Pre-Implemented Forward Movement for Owner.
	UFUNCTION()
	void MoveForward(float AxisValue);

	// Pre-Implemented Right Movement for Owner.
	UFUNCTION()
	void MoveRight(float AxisValue);

	// Pre-Implemented Begin Sprint for Owner.
	UFUNCTION()
	void BeginSprint();

	// Pre-Implemented End Sprint for Owner.
	UFUNCTION()
	void EndSprint();

	// Pre-Implemented Toggle Crouch for Owner.
	UFUNCTION()
	void ToggleCrouch();

	// Pre-Implemented Turn Right for Owner.
	UFUNCTION()
	void TurnRight(float AxisValue);

	// Pre-Implemented Turn Up for Owner.
	UFUNCTION()
	void TurnUp(float AxisValue);

public:	

	// Take Axis / Input Keys and automatically map the InputComponent accordingly. Any set to NAME_None will be ignored.
	void BindInputs(
		UInputComponent* InputComponent,
		FName ForwardMovementAxis = NAME_None,
		FName RightMovementAxis = NAME_None,
		FName Sprint = NAME_None,
		FName Crouch = NAME_None,
		FName UpTurnAxis = NAME_None,
		FName RightTurnAxis = NAME_None
	);
};
