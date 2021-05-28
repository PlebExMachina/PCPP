// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTAssignLiteral.h"
#include "AI_UTILS.h"

EBTNodeResult::Type UBTTAssignLiteral::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	float FloatingPointValue = AI_UTILS::KeyAsFloat(Entry,Blackboard);

	if (Mode == "=") {
		FloatingPointValue = Value;
	} else if (Mode == "+") {
		FloatingPointValue += Value;
	} else if (Mode == "-") {
		FloatingPointValue -= Value;
	} else if (Mode == "*") {
		FloatingPointValue *= Value;
	} else if (Mode == "/") {
		FloatingPointValue /= Value;
	}

	if (AI_UTILS::AssignNumerical(Entry, Blackboard, Value)) {
		// Success
		return EBTNodeResult::Succeeded;
	}

	// Failure
	return EBTNodeResult::Failed;
}