// Fill out your copyright notice in the Description page of Project Settings.


#include "DDDCharacterMovement.h"
#include "Kismet/KismetMathLibrary.h"

UDDDCharacterMovement::UDDDCharacterMovement() {
	Config.MoveSpeed = 200.f;
	Config.RunMultiplier = 2.0;
	Config.CrouchMultiplier = 0.5;
	Config.LockOnDead = true;
	Config.AimSpeed = 15.f;
}

void UDDDCharacterMovement::SetDDDMovementMode(EDDDMovementMode NewMovementMode){
	// Death Locked Case
	if (Config.LockOnDead && (DDDMovementMode == EDDDMovementMode::DDD_Dead)) {
		return;
	}

	// Update if needed then call relevant delegate.
	if (DDDMovementMode != NewMovementMode) {
		DDDMovementMode = NewMovementMode;
		switch (DDDMovementMode) {
			case EDDDMovementMode::DDD_Walk:
				OnWalk.Broadcast();
				break;
			case EDDDMovementMode::DDD_Run:
				OnRun.Broadcast();
				break;
			case EDDDMovementMode::DDD_Crouch:
				OnCrouch.Broadcast();
				break;
			case EDDDMovementMode::DDD_Dead:
				OnDead.Broadcast();
				break;
		}
	}
}

void UDDDCharacterMovement::SetTargetLocation(FVector NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}

float UDDDCharacterMovement::GetAimYaw()
{
	return CurrentAimYaw;
}

float UDDDCharacterMovement::GetAimPitch()
{
	return CurrentAimPitch;
}

float UDDDCharacterMovement::GetSpeed()
{
	return GetOwner()->GetVelocity().Size();
}

float UDDDCharacterMovement::GetTurn()
{
	return Turn;
}

float UDDDCharacterMovement::GetIdleTime()
{
	return IdleTime;
}

void UDDDCharacterMovement::OnComponentCreated() {
	Super::OnComponentCreated();

	// Initialize movement speed to DDD Method.
	MaxWalkSpeed = Config.MoveSpeed;

	// Initialize rotation so that turn can be tracked correctly from frame 0.
	PreviousActorRotation = GetOwner()->GetActorRotation();

	// Bind movement speed updates so that it will update correctly.
	OnWalk.AddDynamic(this, &UDDDCharacterMovement::__OnWalk);
	OnRun.AddDynamic(this, &UDDDCharacterMovement::__OnRun);
	OnCrouch.AddDynamic(this, &UDDDCharacterMovement::__OnCrouch);
	OnDead.AddDynamic(this, &UDDDCharacterMovement::__OnDead);

	// Binds interrupt so that the component can answer courtesy calls and reset the idle timer.
	MoveInterrupted.AddDynamic(this, &UDDDCharacterMovement::__OnInterrupt);
}

void UDDDCharacterMovement::__OnWalk()
{
	MaxWalkSpeed = Config.MoveSpeed;
}

void UDDDCharacterMovement::__OnRun()
{
	MaxWalkSpeed = Config.MoveSpeed*Config.RunMultiplier;
}

void UDDDCharacterMovement::__OnCrouch()
{
	MaxWalkSpeed = Config.MoveSpeed*Config.CrouchMultiplier;
}

void UDDDCharacterMovement::__OnDead()
{
	MaxWalkSpeed = 0.f;
}

void UDDDCharacterMovement::__OnInterrupt()
{
	IdleTime = 0.f;
}

void UDDDCharacterMovement::__TrackTarget(float DeltaTime)
{
	if (PawnOwner) {
		if (UseControlRotationForAiming) {
			// Interp towards the Control Angle.
			FinalAimPitch = FMath::Clamp(PawnOwner->GetControlRotation().Pitch, -45.f, 45.f);
			FinalAimYaw = FMath::Clamp(PawnOwner->GetControlRotation().Yaw, -45.f, 45.f);
		} else {
			// Get Look At Angle and Interp Towards it.
			auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);
			auto DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, GetOwner()->GetActorRotation());
			FinalAimPitch = FMath::Clamp(DeltaRotator.Pitch, -45.f, 45.f);
			FinalAimYaw = FMath::Clamp(DeltaRotator.Yaw, -45.f, 45.f);
		}
		CurrentAimPitch = UKismetMathLibrary::FInterpTo(CurrentAimPitch, FinalAimPitch, DeltaTime, Config.AimSpeed);
		CurrentAimYaw = UKismetMathLibrary::FInterpTo(CurrentAimYaw, FinalAimYaw, DeltaTime, Config.AimSpeed);
	}
}

void UDDDCharacterMovement::__TrackTurn() 
{
	// Get the difference between the old and new rotation.
	FRotator OldRotation = PreviousActorRotation;
	PreviousActorRotation = GetOwner()->GetActorRotation();
	Turn = UKismetMathLibrary::NormalizedDeltaRotator(OldRotation, PreviousActorRotation).Yaw;
}

void UDDDCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	// If owner moves then reset idle time.
	if (GetSpeed() == 0.f && GetTurn() == 0.f) {
		IdleTime += DeltaTime;
	} else {
		IdleTime = 0.f;
	}
	__TrackTarget(DeltaTime);
	__TrackTurn();
}
