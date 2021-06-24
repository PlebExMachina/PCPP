// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"

ANetworkPlayerController::ANetworkPlayerController() {
	PollingComponent = CreateDefaultSubObject<UPollingClientComponent>(Name(TEXT("PollingComponent")));
}

INetworkPlayerControllerDataStore * ANetworkPlayerController::GetDataStore(){
	return GetWorld()->GetGameInstance<INetworkPlayerDataStore>();
}
