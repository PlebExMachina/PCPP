// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Savable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USavable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Represents an object which can be Saved / Loaded.
 */
class PCPP_INTERFACES_API ISavable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Define save data by creating a JsonObject. JSON is 100% Serializable.
	virtual const FJsonObject Save() {  return FJsonObject(); };

	// Define load behavior of a given JsonObject.
	virtual void Load(const FJsonObject&) { };

	// Default Implementation will just return the name of the object. Overriding will not be neccessary for singletons.
	virtual const FString SaveID() { return Cast<UObject>(this)->GetName(); };
};
