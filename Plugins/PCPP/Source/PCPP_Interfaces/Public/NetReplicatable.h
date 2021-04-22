// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dom/JsonObject.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NetReplicatable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReliableReplicateDelegate, UActorComponent*, Target, bool, Reliable);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNetReplicatable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface which allows implementing components to replicate using UNetReplicate
 */
class PCPP_INTERFACES_API INetReplicatable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
private:
	// Forward declare replication component
	friend class UNetReplicate;

	FReliableReplicateDelegate _ReliableReplicationDelegate;

	// Utilize for ticking of unreliable updates.
	FTimerDelegate _ReplicationTimerDelegate;
	FTimerHandle _ReplicationTimerHandle;

	// Generates a unique tag for the object and on initialization sets it onto the owning component.
	FString _GeneratedTag;
	bool _TagGenerated;
	const FString& _GetTag() {
		if (_TagGenerated) {
			return _GeneratedTag;
		}

		// Generate tag.
		auto Self = Cast<UActorComponent>(this);
		if (Self) {
			// Display names for components must be unique.
			_GeneratedTag = UKismetSystemLibrary::GetDisplayName(Self);
			Self->ComponentTags.Add(FName(*_GeneratedTag));
		}

		return _GeneratedTag;
	}
public:
	/*
	* Create the object that will be used for replication.
	* This is the object that will be both sent and received.
	*/
	virtual const FJsonObject CreateReplicationData() {
		return FJsonObject();
	}

	// For whatever reason FJsonObject is not usable by blueprint functions. :(
	virtual void ReceiveReplicate(const FJsonObject& Data) {}

	// Force an update. (Only way to push forward reliable requests.)
	void ForceUpdate(bool Reliable = true) {
		auto Self = Cast<UActorComponent>(this);
		if (Self) {
			_ReliableReplicationDelegate.Broadcast(Self, Reliable);
		}
	}
};
