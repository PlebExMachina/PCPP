// Fill out your copyright notice in the Description page of Project Settings.


#include "DDDDriver.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UDDDDriver::UDDDDriver()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MovementComponent = nullptr;
	CharacterOwner = nullptr;
	PreviousUpTurnInput = 0.f;
	PreviousRightTurnInput = 0.f;
	LockInputSensitivity = 0.25;
	LockCycleAvailable = true;
	// ...
}


void UDDDDriver::TrackLockOn(AActor * ActorLocked)
{
	if (ActorLocked) {
		LockedOn = true;
	} else {
		LockedOn = false;
	}
}

// Called when the game starts
void UDDDDriver::BeginPlay()
{
	Super::BeginPlay();
	// Bind to Component.
	MovementComponent = Cast<UDDDCharacterMovement>(GetOwner()->GetComponentByClass(UDDDCharacterMovement::StaticClass()));
	LockOnSystem = Cast<ULockOnSystem>(GetOwner()->GetComponentByClass(ULockOnSystem::StaticClass()));
	CharacterOwner = Cast<ACharacter>(GetOwner());

	// Setup turn overrides.
	if (LockOnSystem) {
		LockOnSystem->OnActorLock.AddDynamic(this, &UDDDDriver::TrackLockOn);
	}
}

void UDDDDriver::BindInputs(
	UInputComponent* InputComponent,
	FName ForwardMovementAxis,
	FName RightMovementAxis,
	FName Sprint,
	FName Crouch,
	FName UpTurnAxis,
	FName RightTurnAxis
) {
	if (InputComponent) { 
		if (ForwardMovementAxis != NAME_None) {
			InputComponent->BindAxis(ForwardMovementAxis,this,&UDDDDriver::MoveForward);
		}
		if (RightMovementAxis != NAME_None) {
			InputComponent->BindAxis(RightMovementAxis, this, &UDDDDriver::MoveRight);
		}
		if (Sprint != NAME_None) {
			InputComponent->BindAction(Sprint, EInputEvent::IE_Pressed, this, &UDDDDriver::BeginSprint);
			InputComponent->BindAction(Sprint, EInputEvent::IE_Released, this, &UDDDDriver::EndSprint);
		}
		if (Crouch != NAME_None) {
			InputComponent->BindAction(Crouch, EInputEvent::IE_Pressed, this, &UDDDDriver::ToggleCrouch);
		}
		if (UpTurnAxis != NAME_None) {
			InputComponent->BindAxis(UpTurnAxis, this, &UDDDDriver::TurnUp);
		}
		if (RightTurnAxis != NAME_None) {
			InputComponent->BindAxis(RightTurnAxis, this, &UDDDDriver::TurnRight);
		}
	}
}

void UDDDDriver::MoveForward(float AxisValue)
{
	if (MovementComponent) {
	
	}
}

void UDDDDriver::MoveRight(float AxisValue)
{
	if (MovementComponent) {

	}
}

void UDDDDriver::BeginSprint()
{
	if (MovementComponent) {

	}
}

void UDDDDriver::EndSprint()
{
	if (MovementComponent) {

	}
}

void UDDDDriver::ToggleCrouch()
{
	if (MovementComponent) {

	}
}

void UDDDDriver::TurnRight(float AxisValue)
{
	VerifyLockCycleAvailable();
	if (MovementComponent) {
		if (!LockedOn) {
			if (CharacterOwner) {
				CharacterOwner->AddControllerYawInput(AxisValue);
			}
		} else {
			TryCycleLock(AxisValue);
		}
	}
}

void UDDDDriver::TurnUp(float AxisValue)
{
	VerifyLockCycleAvailable();
	if (MovementComponent) {
		if (!LockedOn) {
			if (CharacterOwner) {
				CharacterOwner->AddControllerPitchInput(AxisValue);
			}
		} else {
			TryCycleLock(AxisValue);
		}
	}
}

void UDDDDriver::TryCycleLock(float AxisValue) {
	bool InputValid = !UKismetMathLibrary::InRange_FloatFloat(AxisValue, LockInputSensitivity*-1.0, LockInputSensitivity);
	if (LockCycleAvailable && InputValid) {
		if (AxisValue > 0.0) {
			LockOnSystem->CycleFurtherLock();
		}
		if (AxisValue < 0.0) {
			LockOnSystem->CycleCloserLock();
		}
		LockCycleAvailable = false;
	}
}

void UDDDDriver::VerifyLockCycleAvailable() {
	bool UpReset = UKismetMathLibrary::InRange_FloatFloat(PreviousUpTurnInput, LockInputSensitivity*-1, LockInputSensitivity);
	bool RightReset = UKismetMathLibrary::InRange_FloatFloat(PreviousRightTurnInput, LockInputSensitivity*-1, LockInputSensitivity);
	if (UpReset && RightReset) {
		LockCycleAvailable = true;
	}
}