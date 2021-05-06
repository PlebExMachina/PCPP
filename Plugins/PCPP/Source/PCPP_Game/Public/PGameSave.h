// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PGameSave.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaveEventOccured);

/**
 * Handles the saving and loading of game files in a simplified way.
 * Anything that implements the Savable interface will be automatically managed whenever SaveGame or LoadGame is called.
 */
UCLASS()
class PCPP_GAME_API UPGameSave : public USaveGame
{
	GENERATED_BODY()

	TMap<FString, FJsonObject> SavedData;

	public:

	// Saves the game to the given slot utilizing information from all Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void SaveFile(FString FileName, int32 UserIndex, int32 RetryCount = 3);

	// Loads the game from the given slot and passes the data to all currently active Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void LoadSave(FString FileName, int32 UserIndex, int32 RetryCount = 3);

	// Passes load data into any Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	// Deletes a save game.
	UFUNCTION(BlueprintCallable)
	void DeleteGame(FString FileName, int32 UserIndex, int32 RetryCount = 3);

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnSaveSucceed;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnSaveFail;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnLoadSucceed;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnLoadFail;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnDeleteSucceed;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnDeleteFail;
};
