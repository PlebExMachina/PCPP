// Fill out your copyright notice in the Description page of Project Settings.


#include "NetReplicate.h"
#include "Serialization/JsonSerializer.h"
#include "..\Public\NetReplicate.h"

UNetReplicate::UNetReplicate()
{
	PrimaryComponentTick.bCanEverTick = false;
	PawnOwner = nullptr;
}

void UNetReplicate::_ProcessReliableRequestFromInterface(UActorComponent * Target, bool Reliable)
{
	if (GetPawnOwner() && GetPawnOwner()->IsLocallyControlled()) {
		auto Interface = Cast<INetReplicatable>(Target);
		if (Interface) {
			// Create heap instance for Shared Pointer through Copy Constructor
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject(Interface->CreateReplicationData()));

			// Write Object to String
			FString OutputString;
			TSharedRef<TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			RequestReplication(Interface->_GetTag(), OutputString);
		}
	}
}

bool UNetReplicate::Validate(const FString& ObjectID, const FString& ReplicateData) {
	return true;
}

INetReplicatable* UNetReplicate::GetCachedComponentByTag(const FString& ObjectID) {
	// Get Existing Cached Component
	auto FoundComp = CachedComponents.Find(ObjectID);
	if (FoundComp) {
		return *FoundComp;
	}

	// Search for component to Cache
	auto comps = GetPawnOwner()->GetComponentsByTag(UActorComponent::StaticClass(), FName(*ObjectID));
	for (auto i = comps.CreateIterator(); i; ++i) {
		auto comp = Cast<INetReplicatable>(*i);
		if (comp) {
			CachedComponents.Add(ObjectID, comp);
			return comp;
		}
	}

	// Failure Case
	return nullptr;
}

void UNetReplicate::ProcessReplicationRequest(const FString& ObjectID, const FString& ReplicateData) {
	if (GetPawnOwner() && !GetPawnOwner()->IsLocallyControlled()) {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ReplicateData);
		TSharedPtr<FJsonObject> JsonParsed;
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
			if (JsonParsed.IsValid()) {
				auto comp = GetCachedComponentByTag(ObjectID);
				if (comp) {
					comp->ReceiveReplicate(*JsonParsed.Get());
				}
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
	if (GetPawnOwner() && GetPawnOwner()->IsLocallyControlled()) {
		if (Reliable) {
			BroadcastServerReliable(ObjectTag, ObjectData);
		} else {
			BroadcastServerUnreliable(ObjectTag, ObjectData);
		}
	}
}

APawn* UNetReplicate::GetPawnOwner() {
	if (PawnOwner) {
		return PawnOwner;
	}
	PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner;
}

void UNetReplicate::RegisterReplication(float ReplicationFrequency, INetReplicatable* ReplicatingComponent) {
	// Sanity Check, Make sure implementing component is actually a component.
	auto TargetAsComponent = Cast<UActorComponent>(ReplicatingComponent);
	if (TargetAsComponent) {
		// Generate Tag if needed. (Tag is needed for all parties.)
		ReplicatingComponent->_GetTag();

		// Second Sanity Check, only perform if owner.
		if (GetPawnOwner() && GetPawnOwner()->IsLocallyControlled()) {
			// Bind implementation to Implementor
			ReplicatingComponent->_ReplicationTimerDelegate.BindLambda([&ReplicatingComponent, this]() {
				// Create heap instance for Shared Pointer through Copy Constructor
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject(ReplicatingComponent->CreateReplicationData()));

				// Write Object to String
				FString OutputString;
				TSharedRef<TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
				FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

				this->RequestReplication(ReplicatingComponent->_GetTag(), OutputString);
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
		}
	}
}
