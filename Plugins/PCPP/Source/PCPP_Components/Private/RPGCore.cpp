// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCore.h"

// Sets default values for this component's properties
URPGCore::URPGCore()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGCore::BindStat(FName Name, FRPGStatConfig Config, EStatDefault Default) {
	// Sanity check, make sure a stat isn't already bound.
	auto ExistingDelegate = _Delegates.Find(Name);
	if (ExistingDelegate) {
		return;
	}

	// Determine default value.
	float DefaultValue = 0.0;
	switch (Default) {
	case EStatDefault::Literal:
		DefaultValue = Config.LiteralDefault;
		break;
	case EStatDefault::Minimum:
		DefaultValue = GetStat(Config.MinimumConstraint);
		break;
	case EStatDefault::Maximum:
		DefaultValue = GetStat(Config.MaximumConstraint);
		break;
	}

	// Bind relevant information.
	_Delegates.Add(Name, FStatUpdatedDelegate());
	_Configs.Add(Name, Config);
	_Statistics.Add(Name, DefaultValue);
	_Dependencies.Add(Name, TSet<FName>());

	// Setup dependencies.
	if (Config.MaximumConstraint != NAME_None) {
		auto DependencyArray = _Dependencies.Find(Config.MaximumConstraint);
		if (DependencyArray) {
			DependencyArray->Add(Name);
		}
	}
	if (Config.MinimumConstraint != NAME_None) {
		auto DependencyArray = _Dependencies.Find(Config.MinimumConstraint);
		if (DependencyArray) {
			DependencyArray->Add(Name);
		}
	}
}

void URPGCore::BindDerivedStat(FName Name, DerivedStatFunctionPtr Definition)
{
	_DerivedStatistics.Add(Name, Definition);
}

float URPGCore::GetDerivedStat(FName Name)
{
	// Ensure it exists.
	auto DerivedStatistic = _DerivedStatistics.Find(Name);
	if (!DerivedStatistic) {
		return 0.f;
	}
	// Sanity check, someone may have set a nullptr. Got to avoid seg-faults!
	if (!(*DerivedStatistic)) {
		return 0.f;
	}
	// Return based on the provided function.
	return (*DerivedStatistic)(_Statistics);
}

float URPGCore::GetStat(FName Name) {
	auto Value = _Statistics.Find(Name);
	if (!Value) {
		return 0.f;
	}
	return *Value;
}

void URPGCore::SetStat(FName Name, float NewValue)
{
	auto Constraints = _Configs.Find(Name);
	if (!Constraints) {
		return;
	}

	// Enforce Constraints
	if (Constraints->MaximumConstraint != NAME_None) {
		if (NewValue > GetStat(Constraints->MaximumConstraint)) {
			NewValue = GetStat(Constraints->MaximumConstraint);
		}
	}
	if (Constraints->MinimumConstraint != NAME_None) {
		if (NewValue < GetStat(Constraints->MinimumConstraint)) {
			NewValue = GetStat(Constraints->MinimumConstraint);
		}
	}

	float OldValue = GetStat(Name);
	_Statistics.Add(Name, NewValue);
	BroadcastDelegate(Name, OldValue);
}

void URPGCore::AddStat(FName Name, float NewValue)
{
	SetStat(Name, GetStat(Name) + NewValue);
}

void URPGCore::SetToMax(FName Name)
{
	auto Constraints = _Configs.Find(Name);
	if (!Constraints) {
		return;
	}
	if (Constraints->MaximumConstraint == NAME_None) {
		return;
	}

	float OldValue = GetStat(Name);
	_Statistics.Add(Name, GetStat(Constraints->MaximumConstraint));
	BroadcastDelegate(Name, OldValue);
}

void URPGCore::SetToMin(FName Name)
{
	auto Constraints = _Configs.Find(Name);
	if (!Constraints) {
		return;
	}
	if (Constraints->MinimumConstraint == NAME_None) {
		return;
	}

	float OldValue = GetStat(Name);
	_Statistics.Add(Name, GetStat(Constraints->MinimumConstraint));
	BroadcastDelegate(Name, OldValue);
}

void URPGCore::BroadcastDelegate(FName Key, float OldValue) {
	float NewValue = GetStat(Key);
	if (OldValue != NewValue) {
		(_Delegates.Find(Key))->Broadcast(OldValue, NewValue);

		// Propagate changes across those dependant.
		auto Dependants = _Dependencies.Find(Key);
		if (Dependants) {
			for (auto It = Dependants->CreateIterator(); It; ++It) {
				// Force those dependant to re-evaluate their constraints.
				SetStat(*It, GetStat(*It));
			}
		}
	}
}

FStatUpdatedDelegate* URPGCore::GetDelegate(FName Name) {
	return _Delegates.Find(Name);
}

void URPGCore::BindCallbackToStat(FName Name, const FStatUpdatedInputDelegate& Callback) {
	auto StatDelegate = GetDelegate(Name);
	if (StatDelegate) {
		StatDelegate->Add(Callback);
	} 
}
