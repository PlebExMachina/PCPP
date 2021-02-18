// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjSkillInstance.generated.h"

/**
 * Represents a generic skill implementations. 
 * All it requires is "Can the skill be executed?" and "What does the skill do?"
 * The details are up to the implemented Subclasses.
 */
UCLASS()
class PCPP_COMPONENTS_API UObjSkillInstance : public UObject
{
	GENERATED_BODY()
	public:
	virtual void SkillImplementation();

	virtual bool CanRun();
};
