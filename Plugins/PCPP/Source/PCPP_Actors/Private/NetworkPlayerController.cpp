// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"

ANetworkPlayerController::ANetworkPlayerController() {
	auto PollingComponentName = FName(TEXT("PollingComponent"));
	PollingComponent = CreateDefaultSubobject<UPollingClientComponent>(PollingComponentName);
}

INetworkPlayerControllerDataStore * ANetworkPlayerController::GetDataStore(){
	return GetWorld()->GetGameInstance<INetworkPlayerControllerDataStore>();
}

void ANetworkPlayerController::ClientGetResponse(const FString & Endpoint, const FJsonObject & Response){
	PollerResponse.Broadcast(Endpoint, FNPCJSON(Response));
}

void ANetworkPlayerController::RegisterResponseFunction(const FString& Endpoint, ResponseFunction Function) {
	ResponseFunctions.Add(Endpoint, Function);
}

FJsonObject ANetworkPlayerController::MakeResponseObject(const FString& Endpoint) {
	if (ResponseFunctions.Find(Endpoint)) {
		// Black Magic, Dereference function pointer pointer, feeding in the player controller. Check the typedef on the header if 
		// whoever is reading this needs to know what is happening.
		return (*(ResponseFunctions.Find(Endpoint)))(this);
	}
	return FJsonObject();
}