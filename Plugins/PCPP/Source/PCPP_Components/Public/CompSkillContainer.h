// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjSkillInstance.h"
#include "CompSkillContainer.generated.h"

/*
* A container meant to contain and manage dynamic skill implementations.
* It is designed to be as generic as possible and to allow as many details as possible to be up to the implementation.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UCompSkillContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCompSkillContainer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TMap<FName, UObjSkillInstance*> _RegisteredSkills;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Registers an implemented skill to a skill slot. Will destroy the previous implementation.
	* Returns a pointer to the new skill instance. 
	* Any existing references to the previous skill implementation should use the new object to prevent seg faults.
	*/ 
	UObjSkillInstance* RegisterSkill(FName SkillSlot, TSubclassOf<UObjSkillInstance> SkillImplementation);

	/*
	* Gets a reference to the existing skill implementation. 
	* It is preferable to handle everything when registering it but there may be cases where this isn't possible.
	*/
	UObjSkillInstance* GetSkill(FName SkillSlot);

	/*
	* Tries to run the skill in the desired slot. Does not report anything, use CanRunSkill for that information.
	*/
	void TryRunSkill(FName SkillSlot);

	/*
	* Checks whether or not the skill can be ran.
	*/
	bool CanRunSkill(FName SkillSlot);
};
