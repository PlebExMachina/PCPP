// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DodgeSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UDodgeSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDodgeSystem();

public:	
	UPROPERTY(EditAnywhere)
	float DodgeStrength;

	UFUNCTION(BlueprintCallable)
	void DodgeInDirection(FVector DodgeDirection);

	UFUNCTION(BlueprintCallable)
	void DodgeUsingInput();

	virtual void BeginPlay() override;

protected:
	ACharacter* CharacterOwner;
	ACharacter* GetCharacterOwner();

	AActor* LockedTarget;	

	UFUNCTION()
	void SetLockedTarget(AActor* NewTarget);

};
