// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGCore.h"
#include "DamageSystem.generated.h"

// Represent damage being inflicted from Attacker to Defender. Also carries an optional "magnitude" and "duration."
typedef void (*DamageFormula)(URPGCore*, URPGCore*, float, float);

/*
* Damage System, allows for the storage of damage formulas which can interact between the owner and target's RPGCore components.
* It uses "In to Out" based logic for damage flow where the Attacker's stats are Projected towards a Defender target.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UDamageSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageSystem();

protected:
	URPGCore* _OwnerRPGCore;

	// Retrieves OwnerRPGCore, initializes if neccessary. (Lazy Evaluation)
	URPGCore* _GetOwnerRPGCore();

	// Global Formula Storage.
	static TMap<FName, DamageFormula> _Formulas;

	// Active damage over time info. (Duration, Magnitude, Formula)
	TArray<DamageFormula> _DOTFormulas;
	TArray<float> _DOTDuration;
	TArray<float> _DOTMagnitude;

	// The attacker inflicting the DOT.
	TArray<URPGCore*> _DOTAttackers;



public:	
	// Required for 'Damage over Time' based approaches.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Registers a damage formula. C++ Only. I recommend loading them all at once while the game is starting up.
	static void RegisterDamageFormula(FName Name,DamageFormula Formula);

	UFUNCTION(BlueprintCallable)
	void TryInflictDamage(AActor* Other, FName Formula = NAME_None, float Magnitude = 0.f, float Duration = 0.f);

	UFUNCTION(BlueprintCallable)
	void TryInflictDamageOverTime(AActor* Other, FName Formula = NAME_None, float Magnitude = 0.f, float Duration = 0.f);
};
