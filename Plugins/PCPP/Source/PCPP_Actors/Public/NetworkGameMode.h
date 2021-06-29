// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuleSet.h"
#include "GameFramework/GameModeBase.h"
#include "NetworkGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PCPP_ACTORS_API ANetworkGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	URuleSet* RuleSet;

public:
	ANetworkGameMode();

	TMap<FName, TArray<FSpawnPoint> > GetTeamSpawnPoints();

	UFUNCTION(BlueprintCallable)
	void LoadRuleSet(TSubclassOf<URuleSet> RuleSetClass);
};
