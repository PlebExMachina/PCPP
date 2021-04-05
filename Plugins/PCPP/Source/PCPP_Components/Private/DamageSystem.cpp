// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"
#include "PCPP_Iterator.h"
#include "PCPP_Tuple.h"

TMap<FName, DamageFormula> UDamageSystem::_Formulas = {};

// Throw away struct to manage iterators and reduce potential programmer errors.
template<typename T1, typename T2, typename T3, typename T4>
struct _DamageSystemIterators {
	_DamageSystemIterators(T1 F, T2 D, T3 M, T4 A) : 
		Values(MakeTuple(F,D,M,A)) {};
	T1& Formula() { return Values.Get<0>(); };
	T2& Duration() { return Values.Get<1>(); };
	T3& Magnitude() { return Values.Get<2>(); };
	T4& Attacker() { return Values.Get<3>(); };
	TTuple<T1, T2, T3, T4> Values;
	// Fake bool conversion. Used for readability in conditional.
	T1 Valid() {
		return Values.Get<0>();
	}
};
template<typename T1, typename T2, typename T3, typename T4>
_DamageSystemIterators<T1, T2, T3, T4> _GetDamageSystemIterators(T1 F, T2 D, T3 M, T4 A) {
	return _DamageSystemIterators<T1, T2, T3, T4>(F,D,M,A);
}

// Sets default values for this component's properties
UDamageSystem::UDamageSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	_OwnerRPGCore = nullptr;
	SetComponentTickEnabled(false);

	_DOTFormulas = {};
	_DOTDuration = {};
	_DOTMagnitude = {};
	_DOTAttackers = {};
}


// Called when the game starts
void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();
	_OwnerRPGCore = Cast<URPGCore>(GetOwner()->GetComponentByClass(URPGCore::StaticClass()));	
}


// Called every frame
void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// No DOTs Stored, End Per Frame process.
	if (_DOTFormulas.Num() == 0) {
		SetComponentTickEnabled(false);
		return;
	}

	// Number of elements are synchronized as they're all initialized simultaniously.
	// Likewise array indices will also be syncrhonized.
	auto Iterators = _GetDamageSystemIterators(
		_DOTFormulas.CreateIterator(), 
		_DOTDuration.CreateIterator(),
		_DOTMagnitude.CreateIterator(),
		_DOTAttackers.CreateIterator()
	);

	// For each DOT
	while (Iterators.Valid()) {
		auto ExecuteFormula = (*(Iterators.Formula()));
		auto Attacker = *(Iterators.Attacker());
		auto Defender = _OwnerRPGCore;
		auto Duration = 0.f;
		auto Magnitude = *(Iterators.Magnitude());
		
		// Get Time Passed (< Delta Time remaining case)
		if ((*(Iterators.Duration()) - DeltaTime) < 0.f) {
			Duration = *(Iterators.Duration());
		} else {
		// Get Time Passed (Normal Case)
			Duration = DeltaTime;
		}

		// Trigger DOT
		ExecuteFormula(Attacker, Defender, Duration, Magnitude);

		// Decrement Time Passed
		*(Iterators.Duration()) -= DeltaTime;

		// Remove Any Expired DOTs
		PCPP_Iterator::RemoveConditional(*(Iterators.Duration()) < 0.f, _TARGS4(Iterators.Values));
	}
}

void UDamageSystem::RegisterDamageFormula(FName Name, DamageFormula Formula)
{
	_Formulas.Add(Name, Formula);
}

void UDamageSystem::TryInflictDamage(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	if (_OwnerRPGCore) {
		auto FormulaInstance = _Formulas.Find(Formula);
		if (FormulaInstance) {
			auto TargetRPGCore = Cast<URPGCore>(Other->GetComponentByClass(URPGCore::StaticClass()));
			if (TargetRPGCore) {
				(*FormulaInstance)(_OwnerRPGCore, TargetRPGCore, Magnitude, Duration);
			}
		}
	}
}

void UDamageSystem::TryInflictDamageOverTime(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	if (_OwnerRPGCore) {
		auto FormulaInstance = _Formulas.Find(Formula);
		if (FormulaInstance) {
			auto TargetSystem = Cast<UDamageSystem>(Other->GetComponentByClass(UDamageSystem::StaticClass()));

			// Append to target's (private) DOT List and then begin DOT processing if it isn't already running.
			if (TargetSystem) {
				TargetSystem->_DOTAttackers.Add(_OwnerRPGCore);
				TargetSystem->_DOTDuration.Add(Duration);
				TargetSystem->_DOTMagnitude.Add(Magnitude);
				TargetSystem->_DOTFormulas.Add(*FormulaInstance);
				TargetSystem->SetComponentTickEnabled(true);
			}
		}
	}
}

