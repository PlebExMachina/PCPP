// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * A collection of common (purely data based) patterns for interacting with UE4.
 */
class DEV_API PCPP_UE4
{
public:
	/*
	*	Retrieves every single loaded object of the input array type.
	*/
	template<typename T>
	static bool GetObjects(TArray<T*>& Out) {
		Out = {};
		for (TObjectIterator<T> Object; Object; ++Object) {
			Out.Add(*Object);
		}
		return Out.Num() > 0;
	};
	/*
	*	A specialization of GetObjects which will instead output to an interface array I.
	*	O = Object Type, I = IInterface
	*/
	template<typename O, typename I>
	static bool GetInterfaces(TArray<I*>& Out) {
		Out = {};
		for (TObjectIterator<O> Object; Object; ++Object) {
			auto i = Cast<I>(*Object);
			if (i) {
				Out.Add(i);
			}
		}
		return Out.Num() > 0;
	};
	/*
	*	Performs an operation using each element of an array.
	*/
	template<typename T, typename F>
	static void ForEach(TArray<T> &In, F Foo) {
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
};
