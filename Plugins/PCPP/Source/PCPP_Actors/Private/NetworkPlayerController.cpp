// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"

ANetworkPlayerController::ANetworkPlayerController() {
	auto PollingComponentName = FName(TEXT("PollingComponent"));
	PollingComponent = CreateDefaultSubobject<UPollingClientComponent>(PollingComponentName);
}

INetworkPlayerControllerDataStore * ANetworkPlayerController::GetDataStore(){
	return GetWorld()->GetGameInstance<INetworkPlayerControllerDataStore>();
}
