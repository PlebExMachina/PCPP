// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnCamera.h"
#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "PCPP_UE4.h"

ULockOnCamera::ULockOnCamera() {
	LockedTarget = nullptr;
	SetComponentTickEnabled(false);
	OwnerController = nullptr;
	CameraTrackSpeed = 4.f;
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnCamera::BeginPlay() {

	UseControlRotationDefault = bUsePawnControlRotation;

	// Bind to existing lock on system. If it doesn't exist it will behave like a normal camera.
	ULockOnSystem* LockOnSystem = nullptr;
	PCPP_UE4::LazyGetCompWithInit(GetOwner(), LockOnSystem, [&](ULockOnSystem* Comp) {
		(Comp->OnActorLock).AddDynamic(this, &ULockOnCamera::LockChanged);
	});

	APawn* PawnOwner = nullptr;
	PCPP_UE4::LazyGetOwnerWithInit(this, PawnOwner, [&](APawn* Owner) {
		OwnerController = Owner->Controller;
	});

}

void ULockOnCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction){
	// Continuously track towards the locked on position if needed.
	if (LockedTarget && OwnerController) {
		FRotator NewCameraRotation = UKismetMathLibrary::RInterpTo(
			OwnerController->GetControlRotation(),
			UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), LockedTarget->GetActorLocation()),
			DeltaTime,
			CameraTrackSpeed
		);
		NewCameraRotation = NewCameraRotation.Add(LockOffset.Pitch, LockOffset.Yaw, LockOffset.Roll);
		OwnerController->SetControlRotation(NewCameraRotation);
	}
}

void ULockOnCamera::LockChanged(AActor * NewLock){
	// Begin Tracking if there is an active target. End Tracking if there is no active target.
	if (NewLock) {
		SetComponentTickEnabled(true);
		bUsePawnControlRotation = true;
	} else {
		SetComponentTickEnabled(false);
		bUsePawnControlRotation = UseControlRotationDefault;
	}
	LockedTarget = NewLock;
}
