// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"
#include "PCPP_UE4.h"

TMap<FName, DamageFormula> UDamageSystem::_Formulas = {};

// Sets default values for this component's properties
UDamageSystem::UDamageSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	_OwnerRPGCore = nullptr;
	SetComponentTickEnabled(false);

	_DOT = {};
}

URPGCore * UDamageSystem::_GetOwnerRPGCore(){
	return PCPP_UE4::LazyGetComp(GetOwner(), _OwnerRPGCore);
}


// Called every frame
void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// No DOTs Stored, End Per Frame process.
	if (_DOT.Num() == 0) { SetComponentTickEnabled(false); return; }

	// If owner RPG Core doesn't exist then exit.
	if (!_GetOwnerRPGCore()) { return; }

	// Process DOTs
	for (auto i = _DOT.CreateIterator(); i; ++i) {
		bool EndOfLife = (i->Duration - DeltaTime) < 0.f;
		auto Duration = EndOfLife ? i->Duration : DeltaTime;

		// Trigger DOT
		(i->Formula)(
			i->Attacker,
			_GetOwnerRPGCore(),
			Duration,
			i->Magnitude
		);

		// Update DOT Lifetime
		if (EndOfLife) {
			i.RemoveCurrent();
		} else {
			i->Duration = i->Duration - DeltaTime;
		}
	}
}

void UDamageSystem::RegisterDamageFormula(FName Name, DamageFormula Formula)
{
	// Add to static (global) formula storage.
	_Formulas.Add(Name, Formula);
}

void UDamageSystem::TryInflictDamage(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	// Owner actually has an RPG Core and the Formula Exists
	// Target has an RPG Core.
	auto FormulaInstance = _Formulas.Find(Formula);
	if (FormulaInstance && _GetOwnerRPGCore()) {
		auto TargetRPGCore = Cast<URPGCore>(Other->GetComponentByClass(URPGCore::StaticClass()));
		if (TargetRPGCore) {
			(*FormulaInstance)(_GetOwnerRPGCore(), TargetRPGCore, Magnitude, Duration);
		}
	}
}

void UDamageSystem::TryInflictDamageOverTime(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	// Owner has RPG Core, Formula Exists, and Target has a Damage System.
	auto FormulaInstance = _Formulas.Find(Formula);
	auto TargetSystem = Cast<UDamageSystem>(Other->GetComponentByClass(UDamageSystem::StaticClass()));
	if (FormulaInstance && _GetOwnerRPGCore() && TargetSystem) {

		// Append to target's (private) DOT List and then begin DOT processing if it isn't already running.
		if (TargetSystem) {
			auto DOTInstance = FDamageDOT(*FormulaInstance, Duration, Magnitude, _GetOwnerRPGCore());
			TargetSystem->_DOT.Add(DOTInstance);
			TargetSystem->SetComponentTickEnabled(true);
		}
	}
}

