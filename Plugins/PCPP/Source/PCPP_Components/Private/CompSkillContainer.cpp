// Fill out your copyright notice in the Description page of Project Settings.


#include "CompSkillContainer.h"

// Sets default values for this component's properties
UCompSkillContainer::UCompSkillContainer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCompSkillContainer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCompSkillContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UObjSkillInstance * UCompSkillContainer::RegisterSkill(FName SkillSlot, TSubclassOf<UObjSkillInstance> SkillImplementation){
	// Cleanup if needed. Preventing memory leak.
	auto SkillAlreadyExists = _RegisteredSkills.Find(SkillSlot);
	if (SkillAlreadyExists) {
		(*SkillAlreadyExists)->BeginDestroy();
	}

	UObjSkillInstance* NewSkill = NewObject<UObjSkillInstance>(this, SkillImplementation);
	_RegisteredSkills.Add(SkillSlot, NewSkill);

	return NewSkill;
}

UObjSkillInstance * UCompSkillContainer::GetSkill(FName SkillSlot){
	auto Item = _RegisteredSkills.Find(SkillSlot);
	if (Item) {
		return *Item;
	};
	return nullptr;
}

void UCompSkillContainer::TryRunSkill(FName SkillSlot){
	auto Skill = GetSkill(SkillSlot);
	if (Skill) {
		if (Skill->CanRun()) {
			Skill->SkillImplementation();
		}
	}
}

bool UCompSkillContainer::CanRunSkill(FName SkillSlot){
	auto Skill = GetSkill(SkillSlot);
	if (Skill) {
		return Skill->CanRun();
	}
	return false;
}

