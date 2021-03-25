// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DDDCharacterMovement.generated.h"

UENUM(BlueprintType)
enum class EDDDMovementMode : uint8 {
	DDD_Walk	UMETA(DisplayName = "Walk"),
	DDD_Run		UMETA(DisplayName = "Run"),
	DDD_Crouch	UMETA(DisplayName = "Crouch"),
	DDD_Dead	UMETA(DisplayName = "Dead"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveModeChangeDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveInterruptedDelegate);

USTRUCT(BlueprintType)
struct FDDDCharacterMovementConfig {
	GENERATED_BODY()

	// If set then function calls will do nothing while the component is set to dead.
	UPROPERTY(EditAnywhere)
	bool LockOnDead;

	// The default movement speed. Walking.
	UPROPERTY(EditAnywhere)
	float MoveSpeed;

	// A multiplier that decides how much faster the movement speed should be than walking. >= 1.0 intended.
	UPROPERTY(EditAnywhere)
	float RunMultiplier;

	// A multiplier that decides how much slower the movement speed should be than walking. <= 1.0 intended.
	UPROPERTY(EditAnywhere)
	float CrouchMultiplier;

	// How quickly to track a target for aiming. Lower values will make the movement smoother but slower.
	UPROPERTY(EditAnywhere)
	float AimSpeed;
};

/**
 * Character Movement Component specialized for tracking and managing common 3D data.
 */
UCLASS()
class PCPP_COMPONENTS_API UDDDCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	EDDDMovementMode DDDMovementMode;

	virtual void OnComponentCreated() override;

	UFUNCTION()
	void __OnWalk();

	UFUNCTION()
	void __OnRun();

	UFUNCTION()
	void __OnCrouch();

	UFUNCTION()
	void __OnDead();

	UFUNCTION()
	void __OnInterrupt();

	void __TrackTarget(float DeltaTime);
	void __TrackTurn();

	float FinalAimPitch;
	float FinalAimYaw;
	float CurrentAimPitch;
	float CurrentAimYaw;
	FVector TargetLocation;

	float Turn;
	FRotator PreviousActorRotation;


	float IdleTime;

public:
	UDDDCharacterMovement();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Sets whether or not to use the actor's control rotation for setting Aim Pitch / Aim Yaw instead of a TargetLocation.
	UPROPERTY(BlueprintReadWrite)
	bool UseControlRotationForAiming;

	UPROPERTY(EditAnywhere)
	FDDDCharacterMovementConfig Config;

	// Called when the movement mode changes to walk.
	UPROPERTY(BlueprintAssignable)
	FMoveModeChangeDelegate OnWalk;

	// Called when the movement mode changes to run.
	UPROPERTY(BlueprintAssignable)
	FMoveModeChangeDelegate OnRun;

	// Called when the movement mode changes to crouch.
	UPROPERTY(BlueprintAssignable)
	FMoveModeChangeDelegate OnCrouch;

	// Called when the movement mode changes to dead.
	UPROPERTY(BlueprintAssignable)
	FMoveModeChangeDelegate OnDead;

	/*
	*  Has to be called externally. A courtesy delegate to allow systems to know that some sort of interruption is being made to the movement.
	*  For example when it is called DDDCharacterMovement will know to reset it's IdleTime.
	*  It should be called when staggers, attacks, etc are made since  there is typically no way for the component to know that something has happened
	*  if something is done while it is standing still.
	*/ 
	UPROPERTY(BlueprintAssignable)
	FMoveInterruptedDelegate MoveInterrupted;

	// Sets the movement mode, calls the relevant delegate if the movement mode changes.
	UFUNCTION(BlueprintCallable)
	void SetDDDMovementMode(EDDDMovementMode NewMovementMode);

	// Sets the location to be targeted for Aim Pitch / Aim Yaw.
	void SetTargetLocation(FVector NewTargetLocation);

	// Where the owner is aiming.
	UFUNCTION(BlueprintPure)
	float GetAimYaw();

	// Where the owner is aiming.
	UFUNCTION(BlueprintPure)
	float GetAimPitch();

	// How fast the owner is moving.
	UFUNCTION(BlueprintPure)
	float GetSpeed();

	// How fast the owner is turning.
	UFUNCTION(BlueprintPure)
	float GetTurn();

	UFUNCTION(BlueprintPure)
	float GetIdleTime();

	FORCEINLINE UFUNCTION(BlueprintPure)
	EDDDMovementMode GetDDDMovementMode() { return DDDMovementMode; };
};
