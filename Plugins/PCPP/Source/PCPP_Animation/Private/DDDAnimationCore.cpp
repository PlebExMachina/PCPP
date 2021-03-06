// Fill out your copyright notice in the Description page of Project Settings.


#include "DDDAnimationCore.h"
#include "PCPP_UE4.h"

UDDDAnimationCore::UDDDAnimationCore() {
	TaskIndex = 0;
	OwnerMoveComp = nullptr;
	MovementMode = EDDDMovementMode::DDD_Walk;

	// The way this is configured is probably overkill but it's more to demonstrate how it can be done.
	// The updates can be made less coarse in the AnimBP properties(assuming the default is used.)
	RoundRobinMode = true;
	BatchSize = 2;
	Tasks = {
		&UDDDAnimationCore::__ReplicateSpeed,
		&UDDDAnimationCore::__ReplicateAim,
		&UDDDAnimationCore::__ReplicateIdleTime,
		&UDDDAnimationCore::__ReplicateTurn
	};
}

UDDDCharacterMovement* UDDDAnimationCore::GetMoveComp() {
	return PCPP_UE4::LazyGetCompWithInit(TryGetPawnOwner(), OwnerMoveComp, [&](UDDDCharacterMovement* Comp) {
		(Comp->OnWalk).AddDynamic(this, &UDDDAnimationCore::__OnWalk);
		(Comp->OnRun).AddDynamic(this, &UDDDAnimationCore::__OnRun);
		(Comp->OnCrouch).AddDynamic(this, &UDDDAnimationCore::__OnCrouch);
		(Comp->OnDead).AddDynamic(this, &UDDDAnimationCore::__OnDead);
		MovementMode = Comp->GetDDDMovementMode();
	});
}

void UDDDAnimationCore::NativeUpdateAnimation(float dt) {
	Super::NativeUpdateAnimation(dt);
	if (GetMoveComp()) {
		// Correct dt to account for possible batching.
		dt = TaskTimeElapsed(dt);
		if (RoundRobinMode) {
			// Perform only the requisite amount of tasks in the batch.
			for (auto i = 0; i < BatchSize; ++i) {
				(this->*(Tasks[i]))(dt);
				NextTask();
			}
		// No Queueing, just run every task.
		} else {
			for (auto It = Tasks.CreateIterator(); It; ++It) {
				(this->**It)(dt);
			}
		}
	}
}

void UDDDAnimationCore::NextTask() {
	// Cycle task resetting it if reaching end.
	TaskIndex++;
	if (TaskIndex >= Tasks.Num()) {
		TaskIndex = 0;
	}
}

void UDDDAnimationCore::__OnCrouch() {
	MovementMode = EDDDMovementMode::DDD_Crouch;
}

void UDDDAnimationCore::__OnWalk() {
	MovementMode = EDDDMovementMode::DDD_Walk;
}

void UDDDAnimationCore::__OnRun() {
	MovementMode = EDDDMovementMode::DDD_Run;
}

void UDDDAnimationCore::__OnDead() {
	MovementMode = EDDDMovementMode::DDD_Dead;
}

void UDDDAnimationCore::__ReplicateSpeed(float dt) {
	Speed = OwnerMoveComp->GetSpeed();
}

void UDDDAnimationCore::__ReplicateAim(float dt) {
	AimPitch = OwnerMoveComp->GetAimPitch();
	AimYaw = OwnerMoveComp->GetAimYaw();
}

void UDDDAnimationCore::__ReplicateIdleTime(float dt) {
	IdleTime = OwnerMoveComp->GetIdleTime();
}

void UDDDAnimationCore::__ReplicateTurn(float dt) {
	Turn = OwnerMoveComp->GetTurn();
}
