// Fill out your copyright notice in the Description page of Project Settings.


#include "NetReplicate.h"
#include "Serialization/JsonSerializer.h"
#include "PCPP_UE4.h"

UNetReplicate::UNetReplicate()
{
	PrimaryComponentTick.bCanEverTick = false;
	PawnOwner = nullptr;
}

void UNetReplicate::_ProcessReliableRequestFromInterface(UActorComponent * Target, bool Reliable)
{
	PCPP_UE4::Network::Local(GetPawnOwner(), [&]() {
		auto Interface = Cast<INetReplicatable>(Target);
		if (Interface) {
			auto ReplicationData = PCPP_UE4::JSON::ToString(Interface->CreateReplicationData());
			RequestReplication(Interface->_GetTag(), ReplicationData, true);
		}
	});
}

bool UNetReplicate::Validate(const FString& ObjectID, const FString& ReplicateData) {
	return true;
}

INetReplicatable* UNetReplicate::GetCachedComponentByTag(const FString& ObjectID) {
	return PCPP_UE4::GetCachedComponentByTag<UActorComponent, INetReplicatable>(
		GetOwner(), 
		ObjectID, 
		CachedComponents
	);
}

void UNetReplicate::ProcessReplicationRequest(const FString& ObjectID, const FString& ReplicateData) {
	if (GetPawnOwner() && !GetPawnOwner()->IsLocallyControlled()) {
		FJsonObject Data;
		if (PCPP_UE4::JSON::ToObject(ReplicateData, Data)) {
			auto comp = GetCachedComponentByTag(ObjectID);
			if (comp) {
				comp->ReceiveReplicate(Data);
			}
		}
	}
}

void UNetReplicate::BroadcastMulticastUnreliable_Implementation(const FString& ObjectID, const FString& ReplicateData) {
	ProcessReplicationRequest(ObjectID, ReplicateData);
}
bool UNetReplicate::BroadcastMulticastUnreliable_Validate(const FString& ObjectID, const FString& ReplicateData) { 
	return true;
}

void UNetReplicate::BroadcastMulticastReliable_Implementation(const FString& ObjectID, const FString& ReplicateData) {
	ProcessReplicationRequest(ObjectID, ReplicateData);
}
bool UNetReplicate::BroadcastMulticastReliable_Validate(const FString& ObjectID, const FString& ReplicateData) { 
	return true;
}

void UNetReplicate::BroadcastServerUnreliable_Implementation(const FString& ObjectID, const FString& ReplicateData) {
	BroadcastMulticastUnreliable(ObjectID, ReplicateData);
}
bool UNetReplicate::BroadcastServerUnreliable_Validate(const FString& ObjectID, const FString& ReplicateData) {
	return Validate(ObjectID, ReplicateData);
}

void UNetReplicate::BroadcastServerReliable_Implementation(const FString& ObjectID, const FString& ReplicateData) {
	BroadcastMulticastReliable(ObjectID, ReplicateData);
}
bool UNetReplicate::BroadcastServerReliable_Validate(const FString& ObjectID, const FString& ReplicateData) {
	return Validate(ObjectID, ReplicateData);
}

void UNetReplicate::RequestReplication(const FString& ObjectTag, const FString& ObjectData, bool Reliable) {
	PCPP_UE4::Network::Local(GetPawnOwner(), [&]() {
		if (Reliable) {
			BroadcastServerReliable(ObjectTag, ObjectData);
		}
		else {
			BroadcastServerUnreliable(ObjectTag, ObjectData);
		}
	});
}

APawn* UNetReplicate::GetPawnOwner() {
	return PCPP_UE4::LazyGetOwner(this, PawnOwner);
}

void UNetReplicate::RegisterReplication(float ReplicationFrequency, INetReplicatable* ReplicatingComponent) {
	// Sanity Check, Make sure implementing component is actually a component.
	auto TargetAsComponent = Cast<UActorComponent>(ReplicatingComponent);
	if (TargetAsComponent) {
		// Generate Tag if needed. (Tag is needed for all parties.)
		ReplicatingComponent->_GetTag();

		// Second Sanity Check, only perform locally
		PCPP_UE4::Network::Local(GetPawnOwner(), [&]() {
			// Bind implementation to Implementor
			ReplicatingComponent->_ReplicationTimerDelegate.BindLambda([&ReplicatingComponent, this]() {
				// Create heap instance for Shared Pointer through Copy Constructor
				auto Data = PCPP_UE4::JSON::ToString(ReplicatingComponent->CreateReplicationData());

				this->RequestReplication(ReplicatingComponent->_GetTag(), Data);
			});

			// Begin Timer Event on Implementor's Behalf
			GetWorld()->GetTimerManager().SetTimer(
				ReplicatingComponent->_ReplicationTimerHandle,
				ReplicatingComponent->_ReplicationTimerDelegate,
				ReplicationFrequency,
				(ReplicationFrequency > 0.f), // Only loop if frequency is defined.
				FMath::FRand()	// Some random time between 0 and 1 seconds to reduce scheduling conflicts.
			);

			ReplicatingComponent->_ReliableReplicationDelegate.AddDynamic(this, &UNetReplicate::_ProcessReliableRequestFromInterface);
		});
	}
}
