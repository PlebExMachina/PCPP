// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"

TMap<FName, DamageFormula> UDamageSystem::_Formulas = {};

// Sets default values for this component's properties
UDamageSystem::UDamageSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	OwnerRPGCore = nullptr;
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
	OwnerRPGCore = Cast<URPGCore>(GetOwner()->GetComponentByClass(URPGCore::StaticClass()));	
}


// Called every frame
void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (_DOTFormulas.Num() == 0) {
		SetComponentTickEnabled(false);
		return;
	}

	// Number of elements are synchronized as they're all initialized simultaniously.
	// Likewise array indices will also be syncrhonized.
	auto FormulaIterator = _DOTFormulas.CreateIterator();
	auto DurationIterator = _DOTDuration.CreateIterator();
	auto MagnitudeIterator = _DOTMagnitude.CreateIterator();
	auto AttackerIterator = _DOTAttackers.CreateIterator();

	while (FormulaIterator) {
		// Very small dot time remaining case.
		if ((*DurationIterator - DeltaTime) < 0.f) {
			(*FormulaIterator)(*AttackerIterator, OwnerRPGCore, *DurationIterator, *MagnitudeIterator);
		} else {
		// Normal Case
			(*FormulaIterator)(*AttackerIterator, OwnerRPGCore, DeltaTime, *MagnitudeIterator);
		}


		// Decrement Time Passed
		*DurationIterator -= DeltaTime;

		// Duration Expired Case
		if (*DurationIterator < 0.f) {
			FormulaIterator.RemoveCurrent();
			DurationIterator.RemoveCurrent();
			MagnitudeIterator.RemoveCurrent();
			AttackerIterator.RemoveCurrent();
		} else {
		// Incrementing Case
			++FormulaIterator;
			++DurationIterator;
			++MagnitudeIterator;
			++AttackerIterator;
		}
	}
}

void UDamageSystem::RegisterDamageFormula(FName Name, DamageFormula Formula)
{
	_Formulas.Add(Name, Formula);
}

void UDamageSystem::TryInflictDamage(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	if (OwnerRPGCore) {
		auto FormulaInstance = _Formulas.Find(Formula);
		if (FormulaInstance) {
			auto TargetRPGCore = Cast<URPGCore>(Other->GetComponentByClass(URPGCore::StaticClass()));
			if (TargetRPGCore) {
				(*FormulaInstance)(OwnerRPGCore, TargetRPGCore, Magnitude, Duration);
			}
		}
	}
}

void UDamageSystem::TryInflictDamageOverTime(AActor * Other, FName Formula, float Magnitude, float Duration)
{
	if (OwnerRPGCore) {
		auto FormulaInstance = _Formulas.Find(Formula);
		if (FormulaInstance) {
			auto TargetSystem = Cast<UDamageSystem>(Other->GetComponentByClass(UDamageSystem::StaticClass()));

			// Append to target's (private) DOT List and then begin DOT processing if it isn't already running.
			if (TargetSystem) {
				TargetSystem->_DOTAttackers.Add(OwnerRPGCore);
				TargetSystem->_DOTDuration.Add(Duration);
				TargetSystem->_DOTMagnitude.Add(Magnitude);
				TargetSystem->_DOTFormulas.Add(*FormulaInstance);
				TargetSystem->SetComponentTickEnabled(true);
			}
		}
	}
}

