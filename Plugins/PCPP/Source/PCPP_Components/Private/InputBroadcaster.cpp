// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBroadcaster.h"

UInputBroadcaster::UInputBroadcaster() {
	PrepareInputList(uint8(EDefaultInputLayout::MAX));
}

void UInputBroadcaster::PrepareInputList(uint8 Max) {
	InputList.SetNum(Max,true);
}

FInputBroadcastDelegate* UInputBroadcaster::GetInputDelegate(uint8 Input) {
	if (Input < InputList.Num()) {
		return &(InputList[Input]);
	}
	return nullptr;
}