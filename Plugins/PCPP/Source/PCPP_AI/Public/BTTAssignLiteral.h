// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTAssignLiteral.generated.h"

/**
 * A Task which will asign a literal value for something. Useful for initialization / resetting certain values.
 * Works for any numerical types. (Do not recommend for Enum)
 * Supports the following modes: =,+,-,/,*
 */
UCLASS()
class PCPP_AI_API UBTTAssignLiteral : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Entry;

	UPROPERTY(EditAnywhere)
	FString Mode;

	UPROPERTY(EditAnywhere)
	float Value;
};
