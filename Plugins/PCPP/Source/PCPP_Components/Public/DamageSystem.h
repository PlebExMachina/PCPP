// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGCore.h"
#include "DamageSystem.generated.h"

// Represent damage being inflicted from Attacker to Defender. Also carries an optional "magnitude" and "duration."
typedef void (*DamageFormula)(URPGCore*, URPGCore*, float, float);

USTRUCT()
struct FDamageDOT {
	GENERATED_BODY()

	DamageFormula Formula;
	float Duration;
	float Magnitude;
	URPGCore* Attacker;

	FDamageDOT() {};
	FDamageDOT(DamageFormula f, float d, float m, URPGCore* a) : Formula(f), Duration(d), Magnitude(m), Attacker(a) {};
};

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

	// Active damage over time info.
	TArray<FDamageDOT> _DOT;


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
