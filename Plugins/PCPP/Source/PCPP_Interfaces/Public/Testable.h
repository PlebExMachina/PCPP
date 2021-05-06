// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PCPP_UE4.h"
#include "Testable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTestable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing this interface allows for automated testing.
 */
class PCPP_INTERFACES_API ITestable
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// An array of Tests, return true on pass.
	TArray<bool(*)(ITestable*)> Tests;

	static void RunTests() {
		TArray<UObject*> Array;
		//PCPP_UE4::GetObjects<UObject, ITestable>(Array);
		UE_LOG(LogTemp, Warning, TEXT("%d Testable Objects Found"), Array.Num());

		/*
		for (auto Object = Array.CreateIterator(); Object; ++Object) {
			auto Testable = Cast<ITestable>(*Object);
			for (auto Test = Testable->Tests.CreateIterator(); Test; ++Test) {
				if (!(*Test)(Testable)) {
					UE_LOG(LogTemp, Warning, TEXT("Testable::RunTests FAIL %s TEST %d"), *((*Object)->GetName()), Test.GetIndex());
				}
			}
		}*/
	}
};


