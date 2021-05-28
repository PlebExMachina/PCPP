// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSTrackDT.generated.h"

/**
 * Tracks DT in up to two float entries. One will track "true" dt between service calls and the other will track the accumulated time
 * The service will not reset the accumulator so that must be done with it's own task call.
 */
UCLASS()
class PCPP_AI_API UBTSTrackDT : public UBTService
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector fDT;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector fAccumulator;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
