// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCore.h"

// Sets default values for this component's properties
URPGCore::URPGCore()
{
	PrimaryComponentTick.bCanEverTick = false;
	_CacheKeys = {};
	_CacheValues = {};
	_CacheDelegates = {};
	_CacheDependencies = {};
	_CacheConfigs = {};
	_CacheMinimum = {};
	_CacheMaximum = {};
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

	// Cache is possibly invalidated, rebuild.
	UpdateCache();
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
		OnAnyStatUpdated.Broadcast();
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

void URPGCore::UpdateCache() {
	// Empty Arrays so that they can be refilled in order.
	_CacheValues = {};
	_CacheDelegates = {};
	_CacheDependencies = {};
	_CacheConfigs = {};
	_CacheMinimum = {};
	_CacheMaximum = {};

	// Empty Arrays + In Order Access Invariants imply that indices should be correct.
	for (auto It = _CacheKeys.CreateIterator(); It; ++It) {
		auto Config = _Configs.Find(*It);
		_CacheValues.Add(_Statistics.Find(*It));
		_CacheConfigs.Add(Config);
		_CacheDelegates.Add(_Delegates.Find(*It));
		_CacheDependencies.Add(_Dependencies.Find(*It));
		_CacheMaximum.Add(_Statistics.Find(Config->MaximumConstraint));
		_CacheMinimum.Add(_Statistics.Find(Config->MinimumConstraint));
	}
	// Cache Rebuilt
}

int32 URPGCore::RegisterCacheEntry(FName StatName) {
	auto Index = _CacheKeys.Find(StatName);
	// Already cached. Just return Index.
	if (Index != INDEX_NONE) {
		return Index;
	}

	// Can't cache what doesn't exist.
	auto ItemValue = _Statistics.Find(StatName);
	if (!ItemValue) {
		return INDEX_NONE;
	}

	auto Config = _Configs.Find(StatName);

	// Cache references to all relevant data.
	_CacheKeys.Add(StatName);
	_CacheValues.Add(ItemValue);
	_CacheConfigs.Add(Config);
	_CacheDelegates.Add(_Delegates.Find(StatName));
	_CacheDependencies.Add(_Dependencies.Find(StatName));
	_CacheMaximum.Add(_Statistics.Find(Config->MaximumConstraint));
	_CacheMinimum.Add(_Statistics.Find(Config->MinimumConstraint));

	return Index;
}

float URPGCore::GetCachedStat(int32 Index) {
	// Only Valid Indices
	if ((Index >= 0) || (Index < _CacheKeys.Num())) {
		return *(_CacheValues[Index]);
	}
	return 0.f;
}

void URPGCore::SetCachedStat(int32 Index, float Value) {
	if ((Index >= 0) || (Index < _CacheKeys.Num())) {
		float OldValue = *(_CacheValues[Index]);
		
		// Set Value
		*(_CacheValues[Index]) = Value;

		// Apply Constraints.
		if (_CacheMaximum[Index]) {
			if (*(_CacheValues[Index]) > *(_CacheMaximum[Index])) {
				*(_CacheValues[Index]) = *(_CacheMaximum[Index]);
			}
		}
		if (_CacheMinimum[Index]) {
			if (*(_CacheValues[Index]) < *(_CacheMinimum[Index])) {
				*(_CacheValues[Index]) = *(_CacheMinimum[Index]);
			}
		}

		// Broadcast across delegates.
		float NewValue = *(_CacheValues[Index]);
		if (NewValue != OldValue) {
			(_CacheDelegates[Index])->Broadcast(OldValue, NewValue);
			// Propagate across dependants. It's non-trivial to cache dependants so caching such data is irresponsible.
			// Regardless the case must be handled so that it behaves identically to normal behavior.
			if (_CacheDependencies[Index]) {
				for (auto It = (_CacheDependencies[Index])->CreateIterator(); It; ++It) {
					SetStat(*It, GetStat(*It));
				}
			}
			OnAnyStatUpdated.Broadcast();
		}
	}
}

void URPGCore::AddCachedStat(int32 Index, float Value) {
	SetCachedStat(Index, GetCachedStat(Index) + Value);
}

void URPGCore::SetCachedStatToMax(int32 Index){
	if ((Index >= 0) || (Index < _CacheKeys.Num())) {
		SetCachedStat(Index, *(_CacheMaximum[Index]));
	}
}

void URPGCore::SetCachedStatToMin(int32 Index){
	if ((Index >= 0) || (Index < _CacheKeys.Num())) {
		SetCachedStat(Index, *(_CacheMinimum[Index]));
	}
}
