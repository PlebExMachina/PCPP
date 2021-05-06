// Fill out your copyright notice in the Description page of Project Settings.


#include "TEST_Testable.h"

// Sets default values
ATEST_Testable::ATEST_Testable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	/*Tests = {
		[](ITestable*) {
			UE_LOG(LogTemp, Warning, TEXT("ITestable"));
			return true;
		},
		[](ITestable*) {
			UE_LOG(LogTemp, Warning, TEXT("Tested"));
			return true;
		}
	};*/
}

// Called when the game starts or when spawned
void ATEST_Testable::BeginPlay()
{
	Super::BeginPlay();
//	ITestable::RunTests();
}

// Called every frame
void ATEST_Testable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

