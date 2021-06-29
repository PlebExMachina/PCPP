// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Components/ActorComponent.h"
#include "RuleSet.generated.h"

USTRUCT()
struct PCPP_COMPONENTS_API FSpawnPoint {
	GENERATED_BODY()

		FSpawnPoint() {
		SpawnLocation = nullptr;
		OccupyingPlayer = nullptr;
	}
	FSpawnPoint(APlayerStart* SL) {
		SpawnLocation = SL;
		OccupyingPlayer = nullptr;
	}
	APlayerStart* SpawnLocation;
	APlayerController* OccupyingPlayer;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API URuleSet : public UActorComponent
{
	GENERATED_BODY()


protected:
	TMap<FName, TArray<FSpawnPoint> > TeamSpawnPoints;

public:	
	URuleSet();
	virtual void InitRuleSet(const TMap<FName, TArray<FSpawnPoint> >& SpawnPoints);
		
};
