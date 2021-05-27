// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Comp.generated.h"

/**
 * Represents a decorator node which can handle generic comparisons between common blackboard data types.
 */
UCLASS()
class PCPP_AI_API UComp : public UBTDecorator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector A;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector B;

	UPROPERTY(EditAnywhere)
	FString Mode;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
