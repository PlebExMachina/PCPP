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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UPollingClientComponent : public UActorComponent
{
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

	UFUNCTION(Client, Reliable)
	void ServerReceiveResponse(const FString& Endpoint, const FString& Data, UPollingClientComponent* Caller);

	UFUNCTION(Server, Reliable)
	void ClientReceiveResponse(const FString& Endpoint, const FString& Data);

	UFUNCTION(Client, Reliable)
	void ServerPolled(const FString& Endpoint, UPollingClientComponent* Caller);

	UFUNCTION(Server, Reliable)
	void ClientPolled(const FString& Endpoint);



public:	
	
	void TryPoll(const FString& Endpoint);
	
	virtual void BeginDestroy() override;
};
