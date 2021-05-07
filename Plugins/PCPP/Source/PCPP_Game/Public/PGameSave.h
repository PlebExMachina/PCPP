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

	// The actual saved data.
	TMap<FString, FJsonObject> SavedData;

	// The Master Save File, which contains an index as well as information about other save files.
	static UPGameSave* MasterSave;

	// The user index (only recorded in MasterSave)
	int32 UserIndex;

	public:

	/*==========================================================================================
	* Static Interface (Master Save Object)
	==========================================================================================*/

	// Retrieves the Master Save singleton and loads it if neccessary. Will return nullptr if failed.
	static UPGameSave* GetMasterSave(int32 UserIndex);

	// Commits the currently active Master Save
	static void CommitMasterSave();

	// Checks if the saved game exists.
	UFUNCTION(BlueprintCallable)
	static bool DoesSaveExist(FString FileName);

	// Deletes a save game.
	UFUNCTION(BlueprintCallable)
	static void DeleteFile(FString FileName, int32 RetryCount = 3, bool UnList = true);

	// Creates a save game.
	UFUNCTION(BlueprintCallable)
	static UPGameSave* CreateFile(FString FileName);

	// Clears "vanished" entries on the Master Save.
	UFUNCTION(BlueprintCallable)
	static void FixMasterSave();

	/*==========================================================================================
	* Runtime Interface (Active Save Object)
	==========================================================================================*/

	// Saves the game to the given slot utilizing information from all Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void SaveFile(FString FileName, int32 RetryCount = 3);

	// Loads the game from the given slot and passes the data to all currently active Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void LoadSave(FString FileName, int32 RetryCount = 3);

	// Passes load data into any Savable implementing objects.
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	// Removes unused but loaded data within the Save to optimize RAM usage.
	UFUNCTION(BlueprintCallable)
	void Prune();

	/* 
	* A specialized LoadSave which will load what is missing into the Save but will not call Load on any objects. Returns success state.
	* This will be called automatically when saving to a file but not when only saving to runtime.
	*/
	UFUNCTION(BlueprintCallable)
	bool UnPrune(FString FileName, int32 RetryCount = 3);

	// These delegates will only ever be called when interacting with files. These will only be called on the runtime save instance.
	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnSaveSucceed;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnSaveFail;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnLoadSucceed;

	UPROPERTY(BlueprintAssignable)
	FSaveEventOccured OnLoadFail;

};
