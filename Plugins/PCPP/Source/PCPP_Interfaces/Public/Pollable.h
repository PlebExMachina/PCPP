// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "UObject/Interface.h"
#include "Pollable.generated.h"

class UPollingClientComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPollable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PCPP_INTERFACES_API IPollable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Creates a JSON Object for the purpose of sending a response to a poll endpoint.
	virtual FJsonObject MakeResponseObject(const FString& Endpoint) {
		return FJsonObject();
	}

	// Responds to a JSON Object sent in response (client context.)
	virtual void ClientGetResponse(const FString& Endpoint, const FJsonObject& Response) {
	
	}

	// Responds to a JSON Object sent in response (server context.)
	virtual void ServerGetResponse(const FString& Endpoint, const FJsonObject& Response, UPollingClientComponent* Caller) {
	
	}

};
