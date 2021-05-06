// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Testable.h"
#include "TEST_Testable.generated.h"

/*
* Tests the Testable Interface (By calling two warning.) It will also trigger the tests by existing in the world.
*/
UCLASS()
class DEV_API ATEST_Testable : public AActor//, public ITestable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATEST_Testable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
