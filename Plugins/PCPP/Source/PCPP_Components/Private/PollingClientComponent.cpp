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
	UE_LOG(LogTemp, Warning, TEXT("Begin Play Called"));
	auto GetPollingMode = [&](AActor* Owner) {
		UE_LOG(LogTemp, Warning, TEXT("Get Polling Mode Entered with Owner as, %s"), *(Owner->GetName()));
		if(Cast<AGameModeBase>(Owner)) {
			return EPollingMode::IsServer;
		}
		if (Cast<APlayerController>(Owner)) {
			return EPollingMode::IsClient;
		}

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
		auto OwnerImplements = Cast<IPollable>(GetOwner());
		if (OwnerImplements) {
			return OwnerImplements;
		} else {
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

	_Pollers.Add(this);
	// ...
	
}

void UPollingClientComponent::ServerReceiveResponse_Implementation(const FString& Endpoint, const FString& Data, UPollingClientComponent* Caller) {
	UE_LOG(LogTemp, Warning, TEXT("ServerReceiveResponse Inner"));
	if (_Implementation) {
		UE_LOG(LogTemp, Warning, TEXT("ServerReceiveResponse Implementation Found"));
		FJsonObject ResponseObject;
		/////////////////////////////////////////
		UE_LOG(LogTemp, Warning, TEXT("To Object Outer, Input is: %s"), *Data);
		bool ObjectMade = PCPP_UE4::JSON::ToObject(Data, ResponseObject);
		if (ObjectMade) {
			UE_LOG(LogTemp, Warning, TEXT("ServerGetResponse Outer"));
			_Implementation->ServerGetResponse(Endpoint, ResponseObject, Caller);
		}
	}
}


void UPollingClientComponent::ClientReceiveResponse_Implementation(const FString& Endpoint, const FString& Data) {
	UE_LOG(LogTemp, Warning, TEXT("ClientReceiveResponse Inner"));
	if (_Implementation) {
		UE_LOG(LogTemp, Warning, TEXT("ClientReceiveResponse Implementation Found"));
		FJsonObject ResponseObject;
		bool ObjectMade = PCPP_UE4::JSON::ToObject(Data, ResponseObject);
		if (ObjectMade) {
			UE_LOG(LogTemp, Warning, TEXT("ClientGetResponse Outer"));
			_Implementation->ClientGetResponse(Endpoint, ResponseObject);
		}
	}
}

void UPollingClientComponent::ServerPolled_Implementation(const FString & Endpoint, UPollingClientComponent * Caller) {
	UE_LOG(LogTemp, Warning, TEXT("ServerPolled Inner"));
	if (_Implementation) {
		UE_LOG(LogTemp, Warning, TEXT("ServerPolled Implementation Found"));
		auto ResponseObject = _Implementation->MakeResponseObject(Endpoint);
		auto ResponseString = PCPP_UE4::JSON::ToString(ResponseObject);
		UE_LOG(LogTemp, Warning, TEXT("ClientReceiveResponse Outer"));
		Caller->ClientReceiveResponse(Endpoint, ResponseString);
	}
}

void UPollingClientComponent::ClientPolled_Implementation(const FString & Endpoint) {
	UE_LOG(LogTemp, Warning, TEXT("ClientPolled Inner"));
	if (_Implementation) {
		UE_LOG(LogTemp, Warning, TEXT("ClientPolled Implementation Found"));
		auto ResponseObject = _Implementation->MakeResponseObject(Endpoint);
		auto ResponseString = PCPP_UE4::JSON::ToString(ResponseObject);
		auto GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode) {
			auto Comp = Cast<UPollingClientComponent>(GameMode->GetComponentByClass(UPollingClientComponent::StaticClass()));
			if (Comp) {
				UE_LOG(LogTemp, Warning, TEXT("ServerReceiveResponse Outer"));
				Comp->ServerReceiveResponse(Endpoint, ResponseString, this);
			}
		}
	}
}

void UPollingClientComponent::TryPoll(const FString Endpoint) {
	UE_LOG(LogTemp, Warning, TEXT("TryPoll, Entered"));

	// Confirm that code is being run on Client / Server
	bool IsClient = GetWorld()->IsNetMode(NM_Client);
	FString DebugPollMode;
	if (_PollingMode == EPollingMode::NoPoll) {
		DebugPollMode = "NoPoll";
	}
	if (_PollingMode == EPollingMode::IsClient) {
		DebugPollMode = "IsClient";
	}
	if (_PollingMode == EPollingMode::IsServer) {
		DebugPollMode = "IsServer";
	}

	UE_LOG(LogTemp, Warning, TEXT("TryPoll, Polling Mode is %s, IsClient is %d"), *DebugPollMode, IsClient);

	// Sanity Check, don't attempt to poll if an implementation doesn't exist.
	if (_Implementation) {
		UE_LOG(LogTemp, Warning, TEXT("TryPoll, Implementation Found"));
		if (_PollingMode == EPollingMode::IsClient && IsClient) {
			UE_LOG(LogTemp, Warning, TEXT("TryPoll CLIENT MODE"));
			// EXECUTING ON CLIENT, ATTEMPTING TO POLL SERVER
			// POLL SERVER
			UE_LOG(LogTemp, Warning, TEXT("ServerPolled Outer"));
			ServerPolled(Endpoint, this);
		}
		else if (_PollingMode == EPollingMode::IsServer && !IsClient) {
			UE_LOG(LogTemp, Warning, TEXT("TryPoll SERVER MODE"));

			// EXECUTING ON SERVER, ATTEMPTING TO POLL CLIENTS
			for (auto i = _Pollers.CreateIterator(); i; ++i) {
				if ((*i)->_PollingMode == EPollingMode::IsClient) {
					UE_LOG(LogTemp, Warning, TEXT("ClientPolled Outer"));
					(*i)->ClientPolled(Endpoint);
				}
			}
		}
	}
}

void UPollingClientComponent::BeginDestroy() {
	_Pollers.Remove(this);
	Super::BeginDestroy();
}