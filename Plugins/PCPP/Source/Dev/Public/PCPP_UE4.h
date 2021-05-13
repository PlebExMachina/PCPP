// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

/**
 * A collection of common (purely data based) patterns for interacting with UE4.
 */
class DEV_API PCPP_UE4
{
public:
	/*
	*	Retrieves every single loaded object of the input container type.
	*/
	template<template<typename, typename>class Container, class ObjectType, class Allocator>
	static bool GetObjects(Container<ObjectType*, Allocator>& Out) {
		Out = {};
		for (TObjectIterator<ObjectType> Object; Object; ++Object) {
			Out.Add(*Object);
		}
		return Out.Num() > 0;
	};
	/*
	*	A specialization of GetObjects which will instead output to an interface container I.
	*	O = Object Type
	*/
	template<typename ObjectType, template<typename,typename>class C, class InterfaceType, class Allocator>
	static bool GetInterfaces(C<InterfaceType*,Allocator>& Out) {
		Out = {};
		for (TObjectIterator<ObjectType> Object; Object; ++Object) {
			auto i = Cast<InterfaceType>(*Object);
			if (i) {
				Out.Add(i);
			}
		}
		return Out.Num() > 0;
	};
	/*
	*	Performs an operation using each element of an array.
	*/
	template<typename C, typename F>
	static void ForEach(C &In, F Foo) {
		for (auto i = In.CreateIterator(); i; ++i) {
			Foo(*i);
		}
	};
	/*
	*	Retries a function a certain until the function's boolean check succeeds.
	*	For usage pass in a boolean return lambda capture representing what needs to be done.
	*   true = Success, false = Failure.
	*   While not recommended a negative Tries count will execute infinitely until successful.
	*
	*	This is a synchronous action and will freeze the current thread.
	*/
	template<typename F>
	static bool Retry(F Foo, int32 Tries, float WaitTime = 1.f) {
		while (Tries != 0) {
			if (Foo()) {
				return true;
			}
			auto InitTime = FDateTime::Now().ToUnixTimestamp();
			while (FDateTime::Now().ToUnixTimestamp() - InitTime < WaitTime) {
				UE_LOG(LogTemp, Warning, TEXT("Busy Waiting in Retry"));
			}
			Tries--;
		}
		return false;
	}

	// Lazily get component.
	template<typename CompType>
	static CompType* LazyGetComp(AActor* Owner, CompType*& MemberVariable) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Owner) {
			MemberVariable = Cast<CompType>(Owner->GetComponentByClass(CompType::StaticClass()));
		}

		return MemberVariable;
	};

	// Lazily get owner,
	template<typename OwnerType>
	static OwnerType* LazyGetOwner(UActorComponent* Self, OwnerType*& MemberVariable) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Self) {
			MemberVariable = Cast<OwnerType>(Self->GetOwner());
		}

		return MemberVariable;
	}

	// Lazily get component and run init callback if needed.
	template<typename CompType, typename InitCallback>
	static CompType* LazyGetCompWithInit(AActor* Owner, CompType*& MemberVariable, InitCallback Callback){
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Owner) {
			MemberVariable = LazyGetComp(Owner, MemberVariable);

			if (MemberVariable) {
				Callback(MemberVariable);
			}
		}

		return MemberVariable;
	};

	// Lazily get owner and run callback on the retrieved owner.
	template<typename OwnerType, typename InitCallback>
	static OwnerType* LazyGetOwnerWithInit(UActorComponent* Self, OwnerType*& MemberVariable, InitCallback Callback) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Self) {
			MemberVariable = Cast<OwnerType>(Self->GetOwner());
		}

		if (MemberVariable) {
			Callback(MemberVariable);
		}

		return MemberVariable;
	}

	// Perform a callback when a value enters / exits a deadzone. Entering a deadzone is a less typical case so it is optional.
	template<typename F1, typename F2>
	static void DeadzoneAction(float CurrentAxis, float PreviousAxis, float deadzone, F1 ExitDeadzoneCallback, F2 EnterDeadzoneCallback) {
		bool PreviouslyInDeadzone = UKismetMathLibrary::InRange_FloatFloat(PreviousAxis, deadzone*-1.f, deadzone);
		bool CurrentlyInDeadzone = UKismetMathLibrary::InRange_FloatFloat(CurrentAxis, deadzone*-1.f, deadzone);
		if (PreviouslyInDeadzone != CurrentlyInDeadzone) {
			if (CurrentlyInDeadzone) {
				EnterDeadzoneCallback();
			} else {
				ExitDeadzoneCallback();
			}
		}
	}

	class Math {
		public:
		static int32 Mod(int32 a, int32 b) {
			int32 Out = a % b;
			if (Out >= 0) {
				return Out;
			}
			return Out + b;
		}
	};
};
