// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBroadcaster.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "CharacterSelectWidget.generated.h"

/**
 * Routes input logic utilizing InputBroadcaster to extend blueprint behaviors.
 */
UCLASS()
class PCPP_UI_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
	protected:
		void OnUp_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);
		void OnRight_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);
		void OnDown_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);
		void OnLeft_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);
		void OnSubmit_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);
		void OnCancel_Caller(EInputBroadcastState BroadcastState, float BroadcastMagnitude);

		// Binds to appropriate delegates when added to the screen.
		virtual void AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) override;


	public:

		// Performs cleanup before running default behavior.
		virtual void RemoveFromParent() override;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnUp;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnRight;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnDown;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnLeft;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnSubmit;

		UPROPERTY(BlueprintAssignable)
		FInputBroadcastDelegate OnCancel;
};
