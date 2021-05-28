// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSTrackDT.h"
#include "AI_UTILS.h"

void UBTSTrackDT::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds){
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	float AccumulatedTime = AI_UTILS::KeyAsFloat(fAccumulator, Blackboard) + DeltaSeconds;
	
	AI_UTILS::AssignNumerical(fAccumulator, Blackboard, AccumulatedTime);
	AI_UTILS::AssignNumerical(fDT, Blackboard, DeltaSeconds);

}
