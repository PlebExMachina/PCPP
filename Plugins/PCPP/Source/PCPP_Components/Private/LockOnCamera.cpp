// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnCamera.h"
#include "LockOnSystem.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnCamera::ULockOnCamera() {
	LockedTarget = nullptr;
	SetComponentTickEnabled(false);
	OwnerController = nullptr;
	CameraTrackSpeed = 4.f;
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnCamera::BeginPlay() {
	FTimerDelegate TimerCallback;
	FTimerHandle Handle;

	TimerCallback.BindLambda([&] {
		UseControlRotationDefault = bUsePawnControlRotation;
		// Bind to existing lock on system. If it doesn't exist it will behave like a normal camera.
		auto LockOnSystem = Cast<ULockOnSystem>(GetOwner()->GetComponentByClass(ULockOnSystem::StaticClass()));
		if (LockOnSystem) {
			(LockOnSystem->OnActorLock).AddDynamic(this, &ULockOnCamera::LockChanged);
		}
		auto PawnOwner = Cast<APawn>(GetOwner());
		if (PawnOwner) {
			OwnerController = PawnOwner->Controller;
		}
	});

	GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 0.5f, false);
}

void ULockOnCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction){
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

void ULockOnCamera::LockChanged(AActor * NewLock)
{
	UE_LOG(LogTemp, Warning, TEXT("Binding Called"));
	if (!NewLock) {
		SetComponentTickEnabled(false);
		bUsePawnControlRotation = UseControlRotationDefault;
		UE_LOG(LogTemp, Warning, TEXT("Binding nullptr"));
	}
	else {
		SetComponentTickEnabled(true);
		bUsePawnControlRotation = true;
		UE_LOG(LogTemp, Warning, TEXT("Binding Lock Made"));
	}
	LockedTarget = NewLock;
}
