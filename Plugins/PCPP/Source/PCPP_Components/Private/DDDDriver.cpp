// Fill out your copyright notice in the Description page of Project Settings.


#include "DDDDriver.h"
#include "Kismet/KismetMathLibrary.h"
#include "PCPP_UE4.h"

// Sets default values for this component's properties
UDDDDriver::UDDDDriver(){
	PrimaryComponentTick.bCanEverTick = true;
	MovementComponent = nullptr;
	CharacterOwner = nullptr;
	PreviousUpTurnInput = 0.f;
	PreviousRightTurnInput = 0.f;
	LockInputSensitivity = 0.25;
	OwnerCamera = nullptr;
	// ...
}


void UDDDDriver::TrackLockOn(AActor * ActorLocked){
	// Watch LockOnComponent and update lock state accordingly.
	LockedOn = (ActorLocked != nullptr);
}

UDDDCharacterMovement * UDDDDriver::GetMovementComponent(){
	return PCPP_UE4::LazyGetComp(GetOwner(), MovementComponent);
}

UCameraComponent * UDDDDriver::GetOwnerCamera(){
	return PCPP_UE4::LazyGetComp(GetOwner(), OwnerCamera);
}

// Called when the game starts
void UDDDDriver::BeginPlay(){
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
	FName RightTurnAxis) {
	// Bind set of movements to corresponding axis / input names.
	if (InputComponent) { 
		// Movement
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
		// Turning / Camera
		if (UpTurnAxis != NAME_None) {
			InputComponent->BindAxis(UpTurnAxis, this, &UDDDDriver::TurnUp);
		}
		if (RightTurnAxis != NAME_None) {
			InputComponent->BindAxis(RightTurnAxis, this, &UDDDDriver::TurnRight);
		}
	}
}

void UDDDDriver::MoveForward(float AxisValue){
	if (GetMovementComponent() && GetOwnerCamera()) {
		auto CameraRotation = GetOwnerCamera()->GetComponentRotation();
		// Remove pitch bias so that forward vector is correct length.
		CameraRotation.Pitch = 0.f;
		GetMovementComponent()->AddInputVector(UKismetMathLibrary::GetForwardVector(CameraRotation) * AxisValue);
	}
}

void UDDDDriver::MoveRight(float AxisValue){
	if (GetMovementComponent() && GetOwnerCamera()) {
		auto CameraRotation = GetOwnerCamera()->GetComponentRotation();
		// Remove pitch bias so that right vector is correct length.
		CameraRotation.Pitch = 0.f;
		GetMovementComponent()->AddInputVector(UKismetMathLibrary::GetRightVector(CameraRotation) * AxisValue);
	}
}

void UDDDDriver::BeginSprint(){
	if (GetMovementComponent()) {
		GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Run);
	}
}

void UDDDDriver::EndSprint(){
	if (GetMovementComponent()) {
		if (GetMovementComponent()->GetDDDMovementMode() == EDDDMovementMode::DDD_Run) {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Walk);
		}
	}
}

void UDDDDriver::ToggleCrouch(){
	if (GetMovementComponent()) {
		if (GetMovementComponent()->GetDDDMovementMode() == EDDDMovementMode::DDD_Crouch) {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Walk);
		} else {
			GetMovementComponent()->SetDDDMovementMode(EDDDMovementMode::DDD_Crouch);
		}
	}
}

void UDDDDriver::TurnRight(float AxisValue){
	if (GetMovementComponent()) {
		if (!LockedOn) {
			if (GetCharacterOwner()) {
				GetCharacterOwner()->AddControllerYawInput(AxisValue);
			}
		} else {
			PCPP_UE4::DeadzoneAction(AxisValue, PreviousRightTurnInput, LockInputSensitivity, [&]() {
				// Cycle lock only when InDeadzone -> OutsideDeadzone
				TryCycleLock(AxisValue);
			}, []() {});
		}
	}
	PreviousRightTurnInput = AxisValue;
}

void UDDDDriver::TurnUp(float AxisValue){
	if (GetMovementComponent()) {
		if (!LockedOn) {
			if (GetCharacterOwner()) {
				GetCharacterOwner()->AddControllerPitchInput(AxisValue);
			}
		} else {
			// Cycle lock only when InDeadzone -> OutsideDeadzone
			PCPP_UE4::DeadzoneAction(AxisValue, PreviousUpTurnInput, LockInputSensitivity, [&]() {
				TryCycleLock(AxisValue);
			}, []() {});
		}
	}
	PreviousUpTurnInput = AxisValue;
}

void UDDDDriver::TryCycleLock(float AxisValue) {
	bool InputValid = !UKismetMathLibrary::InRange_FloatFloat(AxisValue, LockInputSensitivity*-1.0, LockInputSensitivity);
	if (InputValid) {
		if (AxisValue > 0.0) {
			GetLockOnSystem()->CycleLock(+1);
		}
		if (AxisValue < 0.0) {
			GetLockOnSystem()->CycleLock(-1);
		}
	}
}

ACharacter * UDDDDriver::GetCharacterOwner(){
	return PCPP_UE4::LazyGetOwner(this,CharacterOwner);
}

ULockOnSystem * UDDDDriver::GetLockOnSystem(){
	return PCPP_UE4::LazyGetCompWithInit(GetOwner(), LockOnSystem, [&](ULockOnSystem* Comp) {
		Comp->OnActorLock.AddDynamic(this, &UDDDDriver::TrackLockOn);
	});
}
