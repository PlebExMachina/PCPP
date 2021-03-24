// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LockOnCamera.generated.h"

/**
 * Binds to an existing Lock On System and uses it to automatically track locked targets.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PCPP_COMPONENTS_API ULockOnCamera : public UCameraComponent
{
	GENERATED_BODY()

	public:
	ULockOnCamera();

	// The angle offset that the camera will use when locking.
	UPROPERTY(EditAnywhere)
	FRotator LockOffset;

	// How quickly the camera will track a given target.
	UPROPERTY(EditAnywhere)
	float CameraTrackSpeed;

	// Binds to existing lock on component if possible.
	virtual void BeginPlay() override;

	// Hands tracking of locked positiion.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	protected:
	UFUNCTION()
	void LockChanged(AActor* NewLock);

	private:
	AActor* LockedTarget;
	AController* OwnerController;
	bool UseControlRotationDefault;
};
