// Fill out your copyright notice in the Description page of Project Settings.


#include "PollingClientComponent.h"
#include "GameFramework/GameModeBase.h"
#include "PCPP_UE4.h"

TArray<UPollingClientComponent*> UPollingClientComponent::_Pollers = {};

// Sets default values for this component's properties
UPollingClientComponent::UPollingClientComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	_PollingMode = EPollingMode::NoPoll;
	_Implementation = nullptr;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UPollingClientComponent::BeginPlay()
{
	Super::BeginPlay();
	auto GetPollingMode = [&](AActor* Owner) {
		// GameMode only exists on the server and acts as the sole server authoritative object for polling.
		if(Cast<AGameModeBase>(Owner)) {
			return EPollingMode::IsServer;
		}
		// Player controller exists only on local client and server so it is the authoritative client.
		if (Cast<APlayerController>(Owner)) {
			return EPollingMode::IsClient;
		}

		// As a component it may be that it is attached to a pawn instead of controller, so the owning controller needs to be checked.
		auto PawnOwner = Cast<APawn>(Owner);
		if (PawnOwner) {
			if (Cast<APlayerController>(PawnOwner->GetController())) {
				return EPollingMode::IsClient;
			}
		}
		return EPollingMode::NoPoll;
	};

	auto GetImplementation = [&]() {
		IPollable* Null = nullptr;

		// Check parent for implementation.
		auto OwnerImplements = Cast<IPollable>(GetOwner());
		if (OwnerImplements) {
			return OwnerImplements;
		} else {
			// Checks siblings for implementation.
			for(auto i = (GetOwner()->GetComponents()).CreateConstIterator(); i; ++i) {
				auto CompImplements = Cast<IPollable>(*i);
				if (CompImplements) {
					return CompImplements;
				}
			}
		}
		return Null;  
	};

	_PollingMode = GetPollingMode(GetOwner());
	_Implementation = GetImplementation();

	// Add to global pollers list for server sided poll requests.
	_Pollers.Add(this);
	// ...
	
}

void UPollingClientComponent::ServerReceiveResponse_Implementation(const FString& Endpoint, const FString& Data, UPollingClientComponent* Caller) {
	if (_Implementation) {
		FJsonObject ResponseObject;
		bool ObjectMade = PCPP_UE4::JSON::ToObject(Data, ResponseObject);
		if (ObjectMade) {
			_Implementation->ServerGetResponse(Endpoint, ResponseObject, Caller);
		}
	}
}


void UPollingClientComponent::ClientReceiveResponse_Implementation(const FString& Endpoint, const FString& Data) {
	if (_Implementation) {
		FJsonObject ResponseObject;
		bool ObjectMade = PCPP_UE4::JSON::ToObject(Data, ResponseObject);
		if (ObjectMade) {
			_Implementation->ClientGetResponse(Endpoint, ResponseObject);
		}
	}
}

void UPollingClientComponent::ServerPolled_Implementation(const FString & Endpoint, UPollingClientComponent * Caller) {
	if (_Implementation) {
		auto ResponseObject = _Implementation->MakeResponseObject(Endpoint);
		auto ResponseString = PCPP_UE4::JSON::ToString(ResponseObject);
		// Send data from server back to the caller.
		Caller->ClientReceiveResponse(Endpoint, ResponseString);
	}
}

void UPollingClientComponent::ClientPolled_Implementation(const FString & Endpoint) {
	if (_Implementation) {
		auto ResponseObject = _Implementation->MakeResponseObject(Endpoint);
		auto ResponseString = PCPP_UE4::JSON::ToString(ResponseObject);

		// Data received from client, but executing on server. So GameMode exists in this context.
		auto GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode) {
			auto Comp = Cast<UPollingClientComponent>(GameMode->GetComponentByClass(UPollingClientComponent::StaticClass()));
			if (Comp) {
				// Because the client was polled, the server should receive the response.
				Comp->ServerReceiveResponse(Endpoint, ResponseString, this);
			}
		}
	}
}

void UPollingClientComponent::TryPoll(const FString Endpoint) {
	// Confirm that code is being run on Client / Server
	bool IsClient = GetWorld()->IsNetMode(NM_Client);

	// Sanity Check, don't attempt to poll if an implementation doesn't exist.
	if (_Implementation) {
		if (_PollingMode == EPollingMode::IsClient && IsClient) {
			// EXECUTING ON CLIENT, ATTEMPTING TO POLL SERVER
			// POLL SERVER
			ServerPolled(Endpoint, this);
		}
		else if (_PollingMode == EPollingMode::IsServer && !IsClient) {
			// EXECUTING ON SERVER, ATTEMPTING TO POLL CLIENTS
			for (auto i = _Pollers.CreateIterator(); i; ++i) {
				if ((*i)->_PollingMode == EPollingMode::IsClient) {
					(*i)->ClientPolled(Endpoint);
				}
			}
		}
	}
}

void UPollingClientComponent::BeginDestroy() {
	// Remove self from global pollers list.
	_Pollers.Remove(this);
	Super::BeginDestroy();
}