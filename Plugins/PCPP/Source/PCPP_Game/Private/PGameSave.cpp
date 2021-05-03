// Fill out your copyright notice in the Description page of Project Settings.


#include "PGameSave.h"
#include "Savable.h"
#include "Kismet/GameplayStatics.h"

void UPGameSave::SaveGame(FString FileName, int32 UserIndex, int32 RetryCount) {
	// Create instance to actually commit to memory.
	UPGameSave* SaveGameInstance = Cast<UPGameSave>(UGameplayStatics::CreateSaveGameObject(UPGameSave::StaticClass()));
	SaveGameInstance->SavedData = SavedData;

	TArray<UObject*> Array;
	for (TObjectIterator<UObject> Object; Object; ++Object) {
		if (Object->Implements<USavable>()) {
			Array.Add(*Object);
		}
	}
	
	for (auto Object = Array.CreateIterator(); Object; ++Object) {
		auto Savable = Cast<ISavable>(*Object);
		// Add to Instance as well as local (probably) runtime object.
		SavedData.Add(Savable->SaveID(),Savable->Save());
		SaveGameInstance->SavedData.Add(Savable->SaveID(), Savable->Save());
	}

	bool SaveSuccessful = UGameplayStatics::SaveGameToSlot(SaveGameInstance, FileName, UserIndex);
	if (SaveSuccessful) {
		OnSaveSucceed.Broadcast();
	} else {
		if (RetryCount >= 0) {
			// Busy waiting 1s (synchronous) to freeze engine to allow whatever is up with the file to be resolved.
			auto InitTime = FDateTime::Now().ToUnixTimestamp();
			while (FDateTime::Now().ToUnixTimestamp() - InitTime < 1) {
				UE_LOG(LogTemp, Warning, TEXT("Busy Waiting in Save Game"));
			}
			SaveGame(FileName, UserIndex, RetryCount - 1);
		} else {
			OnSaveFail.Broadcast();
		}
	}
}

void UPGameSave::LoadGame(FString FileName, int32 UserIndex, int32 RetryCount) {
	 UPGameSave* LoadGameInstance = Cast<UPGameSave>(UGameplayStatics::LoadGameFromSlot(FileName,UserIndex));
	 if (LoadGameInstance) {
		 SavedData = LoadGameInstance->SavedData;
		 TArray<UObject*> Array;
		 for (TObjectIterator<UObject> Object; Object; ++Object) {
			 if (Object->Implements<USavable>()) {
				 Array.Add(*Object);
			 }
		 }

		 for (auto Object = Array.CreateIterator(); Object; ++Object) {
			 auto Savable = Cast<ISavable>(*Object);
			 auto SaveData = SavedData.Find(Savable->SaveID());
			 if (SaveData) {
				 Savable->Load(*SaveData);
			 }
			 else {
				 Savable->Load(FJsonObject());
			 }
		 }
		 OnLoadSucceed.Broadcast();
	 } else {
		 if (RetryCount >= 0) {
			// Busy waiting 1s (synchronous) to freeze engine to allow whatever is up with the file to be resolved.
			 auto InitTime = FDateTime::Now().ToUnixTimestamp();
			 while (FDateTime::Now().ToUnixTimestamp() - InitTime < 1) {
				UE_LOG(LogTemp, Warning, TEXT("Busy Waiting in Load Game"));
			 }
			 LoadGame(FileName, UserIndex, RetryCount - 1);
		 } else {
			 OnLoadFail.Broadcast();
		 }
	 }
}

void UPGameSave::LoadGameFromRuntime() {
	TArray<UObject*> Array;
	for (TObjectIterator<UObject> Object; Object; ++Object) {
		if (Object->Implements<USavable>()) {
			Array.Add(*Object);
		}
	}

	for (auto Object = Array.CreateIterator(); Object; ++Object) {
		auto Savable = Cast<ISavable>(*Object);
		auto SaveData = SavedData.Find(Savable->SaveID());
		if (SaveData) {
			Savable->Load(*SaveData);
		}
		else {
			Savable->Load(FJsonObject());
		}
	}
}

void UPGameSave::DeleteGame(FString FileName, int32 UserIndex, int32 RetryCount) {
	bool DeleteSuccessful = UGameplayStatics::DeleteGameInSlot(FileName, UserIndex);
	if (DeleteSuccessful) {
		OnDeleteSucceed.Broadcast();
	} else {
		if (RetryCount >= 0) {
			// Busy waiting 1s (synchronous) to freeze engine to allow whatever is up with the file to be resolved.
			auto InitTime = FDateTime::Now().ToUnixTimestamp();
			while (FDateTime::Now().ToUnixTimestamp() - InitTime < 1) {
				UE_LOG(LogTemp, Warning, TEXT("Busy Waiting in Delete Game"));
			}
			DeleteGame(FileName, UserIndex, RetryCount - 1);
		} else {
			OnDeleteFail.Broadcast();
		}
	}
}