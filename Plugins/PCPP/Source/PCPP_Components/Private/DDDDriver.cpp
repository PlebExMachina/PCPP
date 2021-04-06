// Fill out your copyright notice in the Description page of Project Settings.


#include "DDDDriver.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UDDDDriver::UDDDDriver()
{
	PrimaryComponentTick.bCanEverTick = true;
	MovementComponent = nullptr;
	CharacterOwner = nullptr;
	PreviousUpTurnInput = 0.f;
	PreviousRightTurnInput = 0.f;
	LockInputSensitivity = 0.25;
	OwnerCamera = nullptr;
	LockCycleAvailable = true;
	// ...
}


void UDDDDriver::TrackLockOn(AActor * ActorLocked)
{
	// Watch LockOnComponent and update lock state accordingly.
	if (ActorLocked) {
		LockedOn = true;
	} else {
		LockedOn = false;
	}
}

UDDDCharacterMovement * UDDDDriver::GetMovementComponent()
{
	if(!MovementComponent){
		MovementComponent = Cast<UDDDCharacterMovement>(GetOwner()->GetComponentByClass(UDDDCharacterMovement::StaticClass()));
	}
	return MovementComponent;
}

UCameraComponent * UDDDDriver::GetOwnerCamera()
{
	if (!OwnerCamera) {
		OwnerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	}
	return OwnerCamera;
}

// Called when the game starts
void UDDDDriver::BeginPlay()
{
	Super::BeginPlay();
	GetMovementComponent();
	GetCharacterOwner();
	GetLockOnSystem();
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
	// Bind set of movements to corresponding axis / input names.
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
	if (GetMovementComponent() && GetOwnerCamera()) {
		auto CameraRotation = GetOwnerCamera()->GetComponentRotation();
		CameraRotation.Pitch = 0.f;
		GetMovementComponent()->AddInputVector(UKismetMathLibrary::GetForwardVector(CameraRotation) * AxisValue);
	}
}

void UDDDDriver::MoveRight(float AxisValue)
{
	if (GetMovementComponent() && GetOwnerCamera()) {
		auto CameraRotation = GetOwnerCamera()->GetComponentRotation();
		CameraRotation.Pitch = 0.f;
		GetMovementComponent()->AddInputVector(UKismetMathLibrary::GetRightVector(CameraRotation) * AxisValue);
	}
}

void UDDDDriver::BeginSprint()
{
	if (GetMovementComponent()) {
		GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Run);
	}
}

void UDDDDriver::EndSprint()
{
	if (GetMovementComponent()) {
		if (GetMovementComponent()->GetDDDMovementMode() == EDDDMovementMode::DDD_Run) {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Walk);
		}
	}
}

void UDDDDriver::ToggleCrouch()
{
	if (GetMovementComponent()) {
		if (GetMovementComponent()->GetDDDMovementMode() == EDDDMovementMode::DDD_Crouch) {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Walk);
		} else {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Crouch);
		}
	}
}

void UDDDDriver::TurnRight(float AxisValue)
{
	VerifyLockCycleAvailable();
	if (GetMovementComponent()) {
		if (!LockedOn) {
			if (GetCharacterOwner()) {
				GetCharacterOwner()->AddControllerYawInput(AxisValue);
			}
		} else {
			TryCycleLock(AxisValue);
		}
	}
}

void UDDDDriver::TurnUp(float AxisValue)
{
	VerifyLockCycleAvailable();
	if (GetMovementComponent()) {
		if (!LockedOn) {
			if (GetCharacterOwner()) {
				GetCharacterOwner()->AddControllerPitchInput(AxisValue);
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
			GetLockOnSystem()->CycleFurtherLock();
		}
		if (AxisValue < 0.0) {
			GetLockOnSystem()->CycleCloserLock();
		}
		LockCycleAvailable = false;
	}
}

ACharacter * UDDDDriver::GetCharacterOwner()
{
	if (!CharacterOwner) {
		CharacterOwner = Cast<ACharacter>(GetOwner());
	}
	return CharacterOwner;
}

ULockOnSystem * UDDDDriver::GetLockOnSystem()
{
	if (!LockOnSystem) {
		LockOnSystem = Cast<ULockOnSystem>(GetOwner()->GetComponentByClass(ULockOnSystem::StaticClass()));
		// Setup turn overrides.
		if (LockOnSystem) {
			LockOnSystem->OnActorLock.AddDynamic(this, &UDDDDriver::TrackLockOn);
		}
	}
	return LockOnSystem;
}

void UDDDDriver::VerifyLockCycleAvailable() {
	bool UpReset = UKismetMathLibrary::InRange_FloatFloat(PreviousUpTurnInput, LockInputSensitivity*-1, LockInputSensitivity);
	bool RightReset = UKismetMathLibrary::InRange_FloatFloat(PreviousRightTurnInput, LockInputSensitivity*-1, LockInputSensitivity);
	if (UpReset && RightReset) {
		LockCycleAvailable = true;
	}
}