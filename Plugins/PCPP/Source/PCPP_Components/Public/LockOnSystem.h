// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "LockOnSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLockMadeDelegate,AActor*, NewLock);

USTRUCT(BlueprintType)
struct FLockOnSystemConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float NearLockDistance;

	UPROPERTY(EditAnywhere)
	float FarLockDistance;

	UPROPERTY(EditAnywhere)
	float LockRadius;

	// Setting this flag will mark that the component is only used for qualifying a lock disabling it's tick.
	UPROPERTY(EditAnywhere)
	bool ReceiveOnly;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API ULockOnSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULockOnSystem();

	UPROPERTY(BlueprintReadWrite)
	bool Lockable;

protected:
	bool LockedOn;
	ULockOnSystem* TargetComponent;

	UPROPERTY(EditAnywhere)
	FLockOnSystemConfig Config;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	FVector GetNearLocation();
	FVector GetFarLocation();

private:
	// Traces and finds lockable targets. Returns whether or not targets were found.
	bool TraceLockableActors(TArray<AActor*>& Out);
	bool CameraMode;
	UCameraComponent* OwnerCamera;

	void SetNewLock(AActor* NewLockReference);

public:
	// Reports on new actors being locked. When lock is ended it will broadcast a nullptr.
	UPROPERTY(BlueprintAssignable)
	FLockMadeDelegate OnActorLock;

	// Hands automatic updates ie: Target is unable to be locked anymore.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Gets Owner's camera and determines lock on mode.
	virtual void BeginPlay() override;

	// Returns whether or not a lock was actually formed.
	UFUNCTION(BlueprintCallable)
	bool BeginLockOn();

	// Ends the lock on if possible.
	UFUNCTION(BlueprintCallable)
	void EndLockOn();

	// Cycles to a closer target.
	UFUNCTION(BlueprintCallable)
	bool CycleLock(int32 IndexOffset);
};
