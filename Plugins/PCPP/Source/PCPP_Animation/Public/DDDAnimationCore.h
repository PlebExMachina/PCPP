// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DDDCharacterMovement.h"
#include "DDDAnimationCore.generated.h"

/**
 * Hooks into existing DDD Movement Component and tracks relevant data.
 * It can also interact with the component locally.
 */
UCLASS(BlueprintType)
class PCPP_ANIMATION_API UDDDAnimationCore : public UAnimInstance
{
	GENERATED_BODY()
	private:
		typedef  void (UDDDAnimationCore::*TaskPtr)(float);

		// The index of the task being run.
		int32 TaskIndex;

		void NextTask();

		UDDDCharacterMovement* OwnerMoveComp;

		FORCEINLINE float TaskTimeElapsed(float dt) { return RoundRobinMode ? (Tasks.Num()*dt)/BatchSize : dt ; };

		void __OnCrouch();
		void __OnWalk();
		void __OnRun();
		void __OnDead();

		// Realized after the fact that these don't actually need dt, but perhaps extended tasks will require it so I'll leave it in.
		void __ReplicateSpeed(float dt);
		void __ReplicateAim(float dt);
		void __ReplicateIdleTime(float dt);
		void __ReplicateTurn(float dt);

	protected:
		// Contains the Tasks located within the component. More specialized animation blueprints can add in extra tasks.
		TArray<TaskPtr> Tasks;

		/*
		* Whether to run the Task updates in Round-Robin mode. 
		*/
		UPROPERTY(EditAnywhere)
		bool RoundRobinMode;

		// How many tasks the run per frame.
		UPROPERTY(EditAnywhere)
		int32 BatchSize;

		// Provides the MoveComp and fetches it if not yet defined.
		UDDDCharacterMovement* GetMoveComp();

		UPROPERTY(BlueprintReadOnly)
		EDDDMovementMode MovementMode;

		UPROPERTY(BlueprintReadOnly)
		float Speed;

		UPROPERTY(BlueprintReadOnly)
		float AimPitch;

		UPROPERTY(BlueprintReadOnly)
		float AimYaw;

		UPROPERTY(BlueprintReadOnly)
		float IdleTime;

		UPROPERTY(BlueprintReadOnly)
		float Turn;

	public:
		UDDDAnimationCore();

		virtual void NativeUpdateAnimation(float dt) override;
};
