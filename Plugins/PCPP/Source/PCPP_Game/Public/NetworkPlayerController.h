// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PollingClientComponent.h"
#include "NetworkPlayerControllerDataStore.h"
#include "Pollable.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PCPP_GAME_API ANetworkPlayerController : public APlayerController, public IPollable
{
	GENERATED_BODY()
public:
	ANetworkPlayerController();
	// Allows for the polling of data from AGameMode
	UPROPERTY()
	UPollingClientComponent* PollingComponent;

	// Retrieves the datastore within the game instance (if it exists.)
	INetworkPlayerControllerDataStore* GetDataStore();
};
