// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetReplicatable.h"
#include "NetReplicate.generated.h"

/*
* Component that handles replication on behalf of the owner.
* It is assumed that the owner is aware of it's own state so the owner alone is unaffected by the multicast events.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PCPP_COMPONENTS_API UNetReplicate : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNetReplicate();

private:
	APawn* PawnOwner;

	TMap<FString, INetReplicatable*> CachedComponents;

	void _ProcessReliableRequestFromInterface(UActorComponent* Target, bool Reliable);

protected:
	// Overridable function to handle validation for the Server RPCs.
	virtual bool Validate(const FString& ObjectID, const FString& ReplicateData);

	/* Retrieves a component as a INetReplicatable interface and caches it for faster access later.
	* If needed it can be further specialized to bypass the hash table lookups.
	*/
	virtual INetReplicatable* GetCachedComponentByTag(const FString& ObjectID);

	// Passes on the request to INetReplicatable for per component processing of the FJsonObject
	void ProcessReplicationRequest(const FString& ObjectID, const FString& ReplicateData);

	APawn* GetPawnOwner();

	// Finally Processes Replication 
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void BroadcastMulticastUnreliable(const FString& ObjectID, const FString& ReplicateData);

	// Finally Processes Replication
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void BroadcastMulticastReliable(const FString& ObjectID, const FString& ReplicateData);

	// Passes to Unreliable Multicast
	UFUNCTION(Server, Unreliable, WithValidation)
	void BroadcastServerUnreliable(const FString& ObjectID, const FString& ReplicateData);

	// Passes to Reliable Multicast
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void BroadcastServerReliable(const FString& ObjectID, const FString& ReplicateData);

public:	

	/* Request for the replication of a JSON String across clients for an Object with the given tag.
	* Automatically routes to the appropriate RPC.
	*/
	void RequestReplication(const FString& ObjectTag, const FString& ObjectData, bool Reliable = false);

	/*
	* Register the implementor of the INetReplicatable interface to begin replication.
	*/
	void RegisterReplication(float ReplicationFrequency, INetReplicatable* ReplicatingComponent);

};
