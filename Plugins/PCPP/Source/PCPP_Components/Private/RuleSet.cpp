// Fill out your copyright notice in the Description page of Project Settings.


#include "RuleSet.h"


// Sets default values for this component's properties
URuleSet::URuleSet()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void URuleSet::InitRuleSet(const TMap<FName, TArray<FSpawnPoint>>& SpawnPoints)
{
	TeamSpawnPoints = SpawnPoints;
}
