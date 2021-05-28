// Fill out your copyright notice in the Description page of Project Settings.


#include "Comp.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI_UTILS.h"

bool UBTDComp::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	FString Function = Mode.ToLower();

	float fA = AI_UTILS::KeyAsFloat(A, Blackboard);
	float fB = AI_UTILS::KeyAsFloat(B, Blackboard);

	if (Function == "lt") {
		return fA < fB;
	}

	if (Function == "lte") {
		return fA <= fB;
	}

	if (Function == "gt") {
		return fA > fB;
	}

	if (Function == "gte") {
		return fA >= fB;
	}

	if (Function == "eq") {
		return fA == fB;
	}

	// Failure Case
	return false;
}