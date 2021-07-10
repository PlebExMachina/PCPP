// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectWidget.h"

void UCharacterSelectWidget::AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) {
	Super::AddToScreen(LocalPlayer, ZOrder);
	// Binding
	auto Owner = GetOwningPlayer();
	if (Owner) {
		auto InputBroadcaster = Cast<UInputBroadcaster>(Owner->GetComponentByClass(UInputBroadcaster::StaticClass()));
		if (InputBroadcaster) {
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Cancel)))->AddDynamic(this, &UCharacterSelectWidget::OnCancel_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Confirm)))->AddDynamic(this, &UCharacterSelectWidget::OnSubmit_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Down)))->AddDynamic(this, &UCharacterSelectWidget::OnDown_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Left)))->AddDynamic(this, &UCharacterSelectWidget::OnLeft_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Right)))->AddDynamic(this, &UCharacterSelectWidget::OnRight_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Up)))->AddDynamic(this, &UCharacterSelectWidget::OnUp_Caller);
		}
	}
}

void UCharacterSelectWidget::RemoveFromParent() {
	// Cleanup
	auto Owner = GetOwningPlayer();
	if (Owner) {
		auto InputBroadcaster = Cast<UInputBroadcaster>(Owner->GetComponentByClass(UInputBroadcaster::StaticClass()));
		if (InputBroadcaster) {
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Cancel)))->RemoveDynamic(this, &UCharacterSelectWidget::OnCancel_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Confirm)))->RemoveDynamic(this, &UCharacterSelectWidget::OnSubmit_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Down)))->RemoveDynamic(this, &UCharacterSelectWidget::OnDown_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Left)))->RemoveDynamic(this, &UCharacterSelectWidget::OnLeft_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Right)))->RemoveDynamic(this, &UCharacterSelectWidget::OnRight_Caller);
			(InputBroadcaster->GetInputDelegate(uint8(EDefaultInputLayout::Up)))->RemoveDynamic(this, &UCharacterSelectWidget::OnUp_Caller);
		}
	}
	Super::RemoveFromParent();
}

void UCharacterSelectWidget::OnUp_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnUp.Broadcast(BroadcastState, BroadcastMagnitude);
}

void UCharacterSelectWidget::OnRight_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnRight.Broadcast(BroadcastState, BroadcastMagnitude);
}

void UCharacterSelectWidget::OnDown_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnDown.Broadcast(BroadcastState, BroadcastMagnitude);
}

void UCharacterSelectWidget::OnLeft_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnLeft.Broadcast(BroadcastState, BroadcastMagnitude);
}

void UCharacterSelectWidget::OnSubmit_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnSubmit.Broadcast(BroadcastState, BroadcastMagnitude);
}

void UCharacterSelectWidget::OnCancel_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude) {
	OnCancel.Broadcast(BroadcastState, BroadcastMagnitude);
}
