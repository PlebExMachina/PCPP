// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/BlackboardComponent.h"


/**
 * General utilities for AI, by default UE4 is quite verbose. Hopefully this can simplify things.
 */
class PCPP_AI_API AI_UTILS
{
public:
	static float KeyAsFloat(const FBlackboardKeySelector& Selector, const UBlackboardComponent* Blackboard) {
		auto SelectorType = Blackboard->GetKeyType(Selector.GetSelectedKeyID());

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Float>(Selector.GetSelectedKeyID())) {
			return Blackboard->GetValueAsFloat(Selector.SelectedKeyName);
		}

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Int>(Selector.GetSelectedKeyID())) {
			return Blackboard->GetValueAsInt(Selector.SelectedKeyName) + 0.f;
		}


		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Bool>(Selector.GetSelectedKeyID())) {
			return Blackboard->GetValueAsBool(Selector.SelectedKeyName) + 0.f;
		}

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Enum>(Selector.GetSelectedKeyID())) {
			return Blackboard->GetValueAsEnum(Selector.SelectedKeyName) + 0.f;
		}

		// Failure
		return 0.f;
	}

	// Assigns a value to Numerical data point in the blackboard. Returns whether or not assignment occured.
	static bool AssignNumerical(const FBlackboardKeySelector& Selector, UBlackboardComponent* Blackboard, float Value) {
		auto SelectorType = Blackboard->GetKeyType(Selector.GetSelectedKeyID());

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Float>(Selector.GetSelectedKeyID())) {
			Blackboard->SetValueAsFloat(Selector.SelectedKeyName, Value);
			return true;
		}

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Int>(Selector.GetSelectedKeyID())) {
			Blackboard->SetValueAsInt(Selector.SelectedKeyName, Value + 0);
			return true;
		}

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Bool>(Selector.GetSelectedKeyID())) {
			Blackboard->SetValueAsBool(Selector.SelectedKeyName, Value > 0.f);
			return true;
		}

		if (Blackboard->IsKeyOfType<UBlackboardKeyType_Enum>(Selector.GetSelectedKeyID())) {
			Blackboard->SetValueAsEnum(Selector.SelectedKeyName, Value + 0);
			return true;
		}
		return false;
	}
};
