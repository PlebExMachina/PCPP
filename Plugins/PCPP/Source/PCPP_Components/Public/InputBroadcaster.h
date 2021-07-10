// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputBroadcaster.generated.h"

UENUM(BlueprintType)
enum class EInputBroadcastState : uint8 {
	Pressed,
	Released,
	Held
};

UENUM(BlueprintType)
enum class EDefaultInputLayout : uint8 {
	Up,
	Right,
	Down,
	Left,
	Confirm,
	Cancel,
	// ^ Some modules depend on the above semantics. When using your own set be sure to atleast include these.
	MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInputBroadcastDelegate, EInputBroadcastState, BroadcastState, float, BroadcastMagnitude);

/*
* Handles the broadcasting of inputs from the Controller.
* Allows for safe observation of inputs from outside of the Actor context.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UInputBroadcaster : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Array containing a variable number of Delegates
	TArray<FInputBroadcastDelegate> InputList;

public:	
	UInputBroadcaster();

	/* 
	* Usage, Pass it an enum::MAX value to intiialize the input list.
	* The enum should correspond to controller inputs.
	*/  
	void PrepareInputList(uint8 Max);

	/*
	* Retrieves the delegate (by pointer) corresponding to the enum::ButtonInput
	* This value should not be stored anywhere.
	*/
	FInputBroadcastDelegate* GetInputDelegate(uint8 Input);
};
