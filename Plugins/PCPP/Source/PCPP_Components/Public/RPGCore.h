// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGCore.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatUpdatedDelegate, float, OldValue, float, UpdatedValue);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FStatUpdatedInputDelegate, float, OldValue, float, UpdatedValue);
/*
* The default mode for the stat.
* Literal -> Use the config defined value.
* Otherwise use the value saved in the minimum/maximum key.
*/
UENUM(BlueprintType)
enum class EStatDefault : uint8 {
	Literal,
	Minimum,
	Maximum,
};

USTRUCT(BlueprintType)
struct FRPGStatConfig {
	GENERATED_BODY();

	// Default value if not overridden.
	UPROPERTY(EditAnywhere)
	float LiteralDefault;

	// Sets the minimum value (as an existing stat) that a stat can be.
	UPROPERTY(EditAnywhere)
	FName MinimumConstraint;

	// Sets that maximum value (as an existing stat) that a stat can be.
	UPROPERTY(EditAnywhere)
	FName MaximumConstraint;

	// Bind Raw Pointer Access. Bind to delegate.
	FRPGStatConfig() {
		MinimumConstraint = NAME_None;
		MaximumConstraint = NAME_None;
		LiteralDefault = 0.f;
	}
};

/*
* Dynamically allocates RPG Statistics and provides listeners for their updates.
* If neccessary the data can also be queried directly.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API URPGCore : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGCore();

private:
	void BroadcastDelegate(FName Key, float OldValue);

protected:

	// The actual value of the stat.
	TMap<FName, float > _Statistics;

	// Configuration data for each stat.
	TMap<FName, FRPGStatConfig> _Configs;

	// Delegates used to broadcast changes made to stats.
	TMap<FName, FStatUpdatedDelegate> _Delegates;

	// Keeps track of stats that are directly dependant on the stat in question.
	TMap<FName, TSet<FName>> _Dependencies;

	typedef float(*DerivedStatFunctionPtr)(const TMap<FName, float >&);

	// Keeps track of derived stat
	TMap<FName, DerivedStatFunctionPtr> _DerivedStatistics;

public:

	/*
	* Binds a stat setting up the appropriate constraints.
	* Be aware that order matters. For example you will end up with 0 if you define CurrentHP before MaxHP and set the default to MaxHP.
	* Dependies may not also be set correctly if order is not adhered to.
	*/
	UFUNCTION(BlueprintCallable)
	void BindStat(FName Name, FRPGStatConfig Config, EStatDefault Default = EStatDefault::Literal);

	/* C++ Only, (Blueprint can still Get).
	* Typical usage is to bind a lambda that takes in a "const TMap<FName, float >&" with no captures.
	* Will bind a stat that will be based on existing stats.
	* Nothing is actually stored so constraints of any kind are not respected.
	*/
	void BindDerivedStat(FName Name, DerivedStatFunctionPtr Definition);

	UFUNCTION(BlueprintPure)
	float GetDerivedStat(FName Name);

	// Gets the value of a bound stat.
	UFUNCTION(BlueprintPure)
	float GetStat(FName Name);

	// Sets the value, obeying constraints if they are set.
	UFUNCTION(BlueprintCallable)
	void SetStat(FName Name, float NewValue);

	// Adds to the value, obeying constraints if they are set.
	UFUNCTION(BlueprintCallable)
	void AddStat(FName Name, float NewValue);

	// Sets to the maximum if a constraint is set.
	UFUNCTION(BlueprintCallable)
	void SetToMax(FName Name);

	// Sets to the minimum if a constraint is set.
	UFUNCTION(BlueprintCallable)
	void SetToMin(FName Name);

	// Gets an existing delegate. Returns nullptr on failure.
	FStatUpdatedDelegate* GetDelegate(FName Name);

	UFUNCTION(BlueprintCallable)
	void BindCallbackToStat(FName Name, const FStatUpdatedInputDelegate& Callback);
};
