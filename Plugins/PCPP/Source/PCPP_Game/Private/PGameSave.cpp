// Fill out your copyright notice in the Description page of Project Settings.


#include "PGameSave.h"
#include "Savable.h"
#include "Kismet/GameplayStatics.h"
#include "PCPP_UE4.h"

void UPGameSave::SaveGame() {
	// Get all Savable objects.
	TArray<ISavable*> Array;
	PCPP_UE4::GetInterfaces<UObject, ISavable>(Array);

	// Store all object data into runtime save file instance.
	PCPP_UE4::ForEach(Array, [&](ISavable* i) {
		SavedData.Add(i->SaveID(), i->Save());
	});
}

void UPGameSave::SaveFile(FString FileName, int32 UserIndex, int32 RetryCount) {
	// Store all runtime data.
	SaveGame();

	// Create instance to actually commit to memory.
	UPGameSave* SaveGameInstance = Cast<UPGameSave>(UGameplayStatics::CreateSaveGameObject(UPGameSave::StaticClass()));
	SaveGameInstance->SavedData = SavedData;

	// Try to commit to memory.
	bool SaveSuccessful = PCPP_UE4::Retry([&]() {
		return UGameplayStatics::SaveGameToSlot(SaveGameInstance, FileName, UserIndex);
	}, RetryCount);

	// Report to listeners
	if (SaveSuccessful) {
		OnSaveSucceed.Broadcast();
	} else {
		OnSaveFail.Broadcast();
	}
}

void UPGameSave::LoadSave(FString FileName, int32 UserIndex, int32 RetryCount) {
	// Try Load File
	UPGameSave* LoadGameInstance = nullptr;
	bool LoadSuccessful = PCPP_UE4::Retry([&]() {
		 LoadGameInstance = Cast<UPGameSave>(UGameplayStatics::LoadGameFromSlot(FileName, UserIndex));
		 return LoadGameInstance != nullptr;
	}, RetryCount);

	if (LoadSuccessful) {
		SavedData = LoadGameInstance->SavedData;
		LoadGame();
		OnLoadSucceed.Broadcast();
	} else {
		OnLoadFail.Broadcast();
	}
}

void UPGameSave::LoadGame() {
	TArray<ISavable*> Array;
	PCPP_UE4::GetInterfaces<UObject, ISavable>(Array);
	PCPP_UE4::ForEach(Array, [&](ISavable* i) {
		// Load to each data using either stored data or default object.
		auto SaveData = SavedData.Find(i->SaveID());
		if (SaveData) {
			i->Load(*SaveData);
		} else {
			i->Load(FJsonObject());
		}
	});
}

void UPGameSave::DeleteGame(FString FileName, int32 UserIndex, int32 RetryCount) {
	bool DeleteSuccessful = PCPP_UE4::Retry([&]() {
		return UGameplayStatics::DeleteGameInSlot(FileName, UserIndex);
	}, RetryCount);

	if (DeleteSuccessful) {
		OnDeleteSucceed.Broadcast();
	} else {
		OnDeleteFail.Broadcast();
	}
}