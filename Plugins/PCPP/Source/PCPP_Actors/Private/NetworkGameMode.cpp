// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ANetworkGameMode::ANetworkGameMode() {
	RuleSet = nullptr;
}

TMap<FName, TArray<FSpawnPoint> > ANetworkGameMode::GetTeamSpawnPoints() {
	TMap<FName, TArray<FSpawnPoint> > TeamSpawnPoints = {};
	for (TActorIterator<APlayerStart> It(GetWorld(), APlayerStart::StaticClass()); It; ++It) {
		auto Team = TeamSpawnPoints.Find(It->PlayerStartTag);
		if (!Team) {
			TeamSpawnPoints.Add(It->PlayerStartTag, {});
			Team = TeamSpawnPoints.Find(It->PlayerStartTag);
		}
		Team->Add(FSpawnPoint(*It));
	}
	return TeamSpawnPoints;
}

void ANetworkGameMode::LoadRuleSet(TSubclassOf<URuleSet> RuleSetClass) {
	if (!RuleSet) {
		RuleSet = NewObject<URuleSet>(this, RuleSetClass);
		RuleSet->InitRuleSet(GetTeamSpawnPoints());
	}
}
