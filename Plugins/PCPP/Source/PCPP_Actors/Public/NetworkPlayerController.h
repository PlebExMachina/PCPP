// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PollingClientComponent.h"
#include "NetworkPlayerControllerDataStore.h"
#include "Pollable.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/*
* Simple wrapper to container FJsonObject
*/
USTRUCT()
struct PCPP_ACTORS_API FNPCJSON {
	GENERATED_BODY()

	FNPCJSON() { Inner = {}; }
	FNPCJSON(const FJsonObject& In) { Inner = In; }

	FJsonObject Inner;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerPollResponseDelegate, const FString&, Endpoint, const FNPCJSON&, Response);

/**
 * Contains Server Polling Behavior and Client Side data access for the PlayerController.
 * Also allows the Server to poll the controller.
 */
UCLASS()
class PCPP_ACTORS_API ANetworkPlayerController : public APlayerController, public IPollable
{
	GENERATED_BODY()

	// ResponseFunction type. Pattern is... FJsonObject(ANetworkPlayerController*)
	typedef FJsonObject(*ResponseFunction)(ANetworkPlayerController*);

	TMap<FString, ResponseFunction> ResponseFunctions;

public:
	ANetworkPlayerController();
	// Allows for the polling of data from AGameMode
	UPROPERTY()
	UPollingClientComponent* PollingComponent;

	// Retrieves the datastore within the game instance (if it exists.)
	INetworkPlayerControllerDataStore* GetDataStore();

	// Callback to allow for extendable responses to the poller.
	FPlayerPollResponseDelegate PollerResponse;

	// Responds to a JSON Object sent in response (client context.) Passes it into the response delegate.
	virtual void ClientGetResponse(const FString& Endpoint, const FJsonObject& Response) override;

	// Registers a function (typically Lambda) to generate a JsonObject response based on an Endpoint input.
	void RegisterResponseFunction(const FString& Endpoint, ResponseFunction Function);

	// Generates a Response Package based on a provided definition.
	virtual FJsonObject MakeResponseObject(const FString& Endpoint) override;
};
