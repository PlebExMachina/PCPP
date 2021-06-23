// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetworkPlayerControllerDataStore.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNetworkPlayerControllerDataStore : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PCPP_INTERFACES_API INetworkPlayerControllerDataStore
{
	GENERATED_BODY()

public:

	TMap<FString, bool>				dsBool;
	TMap<FString, int32>			dsInt;
	TMap<FString, float>			dsFloat;
	TMap<FString, FString>			dsString;

	TMap<FString, TArray<bool> >	dsBoolSet;
	TMap<FString, TArray<int32> >	dsIntSet;
	TMap<FString, TArray<float> >	dsFloatSet;
	TMap<FString, TArray<FString> >	dsStringSet;

};

