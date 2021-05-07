// Fill out your copyright notice in the Description page of Project Settings.


#include "PGameSave.h"
#include "Savable.h"
#include "Kismet/GameplayStatics.h"
#include "PCPP_UE4.h"

UPGameSave* UPGameSave::MasterSave = nullptr;

UPGameSave * UPGameSave::GetMasterSave(int32 UserIndex){
	if (MasterSave) {
		return MasterSave;
	}

	// If there's no master save then attempt to create master save until available locking down thread if neccessary.
	if (!UGameplayStatics::DoesSaveGameExist(".MasterSave", UserIndex)) {
		MasterSave = Cast<UPGameSave>(UGameplayStatics::CreateSaveGameObject(UPGameSave::StaticClass()));
		PCPP_UE4::Retry([&]() {
			return UGameplayStatics::SaveGameToSlot(MasterSave, ".MasterSave", UserIndex);
		}, -1);
	}

	// If master save exists but isn't loaded then continue to try to load until successful.
	if (!MasterSave) {
		PCPP_UE4::Retry([&]() {
			MasterSave = Cast<UPGameSave>(UGameplayStatics::LoadGameFromSlot(".MasterSave", UserIndex));
			return MasterSave != nullptr;
		}, -1);
	}
	return MasterSave;
}

void UPGameSave::CommitMasterSave() {
	PCPP_UE4::Retry([&]() {
		return UGameplayStatics::SaveGameToSlot(MasterSave, ".MasterSave", MasterSave->UserIndex);
	}, -1);
}

void UPGameSave::SaveGame() {
	// Get all Savable objects.
	TArray<ISavable*> Array;
	PCPP_UE4::GetInterfaces<UObject>(Array);

	// Store all object data into runtime save file instance.
	PCPP_UE4::ForEach(Array, [&](ISavable* i) {
		SavedData.Add(i->SaveID(), i->Save());
	});
}

void UPGameSave::SaveFile(FString FileName, int32 RetryCount) {
	if (!MasterSave) { return; }

	// Store all runtime data.
	SaveGame();

	// Create instance to actually commit to memory.
	UPGameSave* SaveGameInstance = Cast<UPGameSave>(UGameplayStatics::CreateSaveGameObject(UPGameSave::StaticClass()));
	SaveGameInstance->SavedData = SavedData;

	// Try to commit to memory.
	bool SaveSuccessful = PCPP_UE4::Retry([&]() {
		// UnPrune data to make sure nothing is lost when committing to slot.
		bool UnPruneSuccess = UnPrune(FileName, RetryCount);
		if (!UnPruneSuccess) {
			return false;
		}
		// commit to slot.
		return UGameplayStatics::SaveGameToSlot(SaveGameInstance, FileName, MasterSave->UserIndex);
	}, RetryCount);

	// Report to listeners
	if (SaveSuccessful) {
		OnSaveSucceed.Broadcast();
	} else {
		OnSaveFail.Broadcast();
	}
}

void UPGameSave::LoadSave(FString FileName, int32 RetryCount) {
	if (!MasterSave) { return; }

	// Try Load File
	UPGameSave* LoadGameInstance = nullptr;
	bool LoadSuccessful = PCPP_UE4::Retry([&]() {
		LoadGameInstance = Cast<UPGameSave>(UGameplayStatics::LoadGameFromSlot(FileName, MasterSave->UserIndex));
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
	TArray<UObject*> Objs;
	PCPP_UE4::GetObjects(Objs);

	TArray<ISavable*> Array;
	PCPP_UE4::GetInterfaces<UObject>(Array);
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

void UPGameSave::DeleteFile(FString FileName, int32 RetryCount, bool UnList) {
	if (!MasterSave) { return; }

	// If only unlisting, always succeeds.
	bool DeleteSuccessful = true;

	// Unlist from master save.
	MasterSave->SavedData.Remove(FileName);
	CommitMasterSave();

	// Try to delete for real if not in UnList mode. Not a big deal if it fails.
	if (!UnList) {
		DeleteSuccessful = PCPP_UE4::Retry([&]() {
			return UGameplayStatics::DeleteGameInSlot(FileName, MasterSave->UserIndex);
		}, RetryCount);
	}
}

UPGameSave * UPGameSave::CreateFile(FString FileName)
{
	if (!MasterSave) { return nullptr; }
	if (UGameplayStatics::DoesSaveGameExist(FileName, MasterSave->UserIndex)) {
		return nullptr;
	}

	UPGameSave* NewFile = Cast<UPGameSave>(UGameplayStatics::CreateSaveGameObject(UPGameSave::StaticClass()));

	(MasterSave->SavedData).Add(FileName, FJsonObject());
	CommitMasterSave();

	return NewFile;
}

void UPGameSave::FixMasterSave()
{
	if (!MasterSave) { return; }
	TArray<FString> Files;
	(MasterSave->SavedData).GetKeys(Files);

	PCPP_UE4::ForEach(Files, [&](const FString& i) {
		if (!UGameplayStatics::DoesSaveGameExist(i, MasterSave->UserIndex)) {
			(MasterSave->SavedData).Remove(i);
		}
	});
	CommitMasterSave();
}

void UPGameSave::Prune()
{
	// Get Keys
	TSet<FString> Keys;
	SavedData.GetKeys(Keys);

	// Get All Savable Objects
	TArray<ISavable*> Array;
	PCPP_UE4::GetInterfaces<UObject>(Array);

	// Remove all found Savable keys leaving only unused keys.
	PCPP_UE4::ForEach(Array, [&](ISavable* i) {
		Keys.Remove(i->SaveID());
	});

	// Remove all data associated with unused keys from the Save
	PCPP_UE4::ForEach(Keys, [&](const FString& i) {
		SavedData.Remove(i);
	});
}

bool UPGameSave::DoesSaveExist(FString FileName)
{
	return UGameplayStatics::DoesSaveGameExist(FileName, MasterSave->UserIndex);
}

bool UPGameSave::UnPrune(FString FileName, int32 RetryCount)
{
	if (!MasterSave) { return false; }

	// Try Load File
	UPGameSave* LoadGameInstance = nullptr;
	bool LoadSuccessful = PCPP_UE4::Retry([&]() {
		LoadGameInstance = Cast<UPGameSave>(UGameplayStatics::LoadGameFromSlot(FileName, MasterSave->UserIndex));
		return LoadGameInstance != nullptr;
	}, RetryCount);

	if (LoadSuccessful) {
		// Get all keys that are loaded but not currently in use.
		TSet<FString> LoadedKeys;
		(LoadGameInstance->SavedData).GetKeys(LoadedKeys);

		TArray<FString> SavedKeys;
		SavedData.GetKeys(SavedKeys);

		PCPP_UE4::ForEach(SavedKeys, [&](const FString& i){
			LoadedKeys.Remove(i);
		});

		// Add unused data to the save file.
		PCPP_UE4::ForEach(LoadedKeys, [&](const FString& i) {
			auto MissingData = *(LoadGameInstance->SavedData).Find(i);
			SavedData.Add(i,MissingData);
		});
	}

	return LoadSuccessful;
}
