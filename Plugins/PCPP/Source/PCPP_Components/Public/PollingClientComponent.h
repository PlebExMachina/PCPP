// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pollable.h"
#include "Components/ActorComponent.h"
#include "PollingClientComponent.generated.h"

UENUM(BlueprintType)
enum class EPollingMode : uint8 {
	NoPoll,
	IsClient,
	IsServer,
};

/*
* Acts as a liasion for polling data between Client and Server contexts.
* Auto-detects if it is in Server Context (Component of AGamemode) 
* or Client Context (Component of PlayerController or, Component of Pawn which is controlled by PlayerController)
* 
* The parent or a sibling that implements IPollable will define how to respond to requests as well as how to handle data once
* it is successfully polled.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PCPP_COMPONENTS_API UPollingClientComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPollingClientComponent();

protected:
	// Tracks all local pollers. This is more for the server than the client as it can simplify making multiple client polls.
	static TArray<UPollingClientComponent*> _Pollers;

	// Whether it will attempt to poll the server or attempt to poll the clients.
	EPollingMode _PollingMode;

	// The object containing the implementation for the poller.
	IPollable* _Implementation;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Callback From ClientPolled so that the server receives the polled data.
	UFUNCTION(Client, Reliable)
	void ServerReceiveResponse(const FString& Endpoint, const FString& Data, UPollingClientComponent* Caller);

	// Callback from ServerPolled so that the client receives the polled data.
	UFUNCTION(Server, Reliable)
	void ClientReceiveResponse(const FString& Endpoint, const FString& Data);

	// Client Requests Endpoint, Executes on Server
	UFUNCTION(Client, Reliable)
	void ServerPolled(const FString& Endpoint, UPollingClientComponent* Caller);

	// Server Requests Endpoint, Executes on Client
	UFUNCTION(Server, Reliable)
	void ClientPolled(const FString& Endpoint);



public:
	// Attempts to poll corresponding UPollingClientComponent(s) in the autodetected context (either client or server)
	UFUNCTION(BlueprintCallable)
	void TryPoll(const FString Endpoint);
	
	virtual void BeginDestroy() override;
};
