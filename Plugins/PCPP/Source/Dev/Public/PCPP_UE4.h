// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/JsonSerializer.h"

/**
 * A collection of common (purely data based) patterns for interacting with UE4.
 */
class DEV_API PCPP_UE4
{
public:
	/*
	*	Retrieves every single loaded object of the input container type.
	*/
	template<template<typename, typename>class Container, class ObjectType, class Allocator>
	static bool GetObjects(Container<ObjectType*, Allocator>& Out) {
		Out = {};
		for (TObjectIterator<ObjectType> Object; Object; ++Object) {
			Out.Add(*Object);
		}
		return Out.Num() > 0;
	};
	/*
	*	A specialization of GetObjects which will instead output to an interface container I.
	*	O = Object Type
	*/
	template<typename ObjectType, template<typename,typename>class C, class InterfaceType, class Allocator>
	static bool GetInterfaces(C<InterfaceType*,Allocator>& Out) {
		Out = {};
		for (TObjectIterator<ObjectType> Object; Object; ++Object) {
			auto i = Cast<InterfaceType>(*Object);
			if (i) {
				Out.Add(i);
			}
		}
		return Out.Num() > 0;
	};
	/*
	*	Performs an operation using each element of an array.
	*/
	template<typename C, typename F>
	static void ForEach(C &In, F Foo) {
		for (auto i = In.CreateIterator(); i; ++i) {
			Foo(*i);
		}
	};
	/*
	*	Retries a function a certain until the function's boolean check succeeds.
	*	For usage pass in a boolean return lambda capture representing what needs to be done.
	*   true = Success, false = Failure.
	*   While not recommended a negative Tries count will execute infinitely until successful.
	*
	*	This is a synchronous action and will freeze the current thread.
	*/
	template<typename F>
	static bool Retry(F Foo, int32 Tries, float WaitTime = 1.f) {
		while (Tries != 0) {
			if (Foo()) {
				return true;
			}
			auto InitTime = FDateTime::Now().ToUnixTimestamp();
			while (FDateTime::Now().ToUnixTimestamp() - InitTime < WaitTime) {
				UE_LOG(LogTemp, Warning, TEXT("Busy Waiting in Retry"));
			}
			Tries--;
		}
		return false;
	}

	// Lazily get component.
	template<typename CompType>
	static CompType* LazyGetComp(AActor* Owner, CompType*& MemberVariable) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Owner) {
			MemberVariable = Cast<CompType>(Owner->GetComponentByClass(CompType::StaticClass()));
		}

		return MemberVariable;
	};

	// Lazily get owner,
	template<typename OwnerType>
	static OwnerType* LazyGetOwner(UActorComponent* Self, OwnerType*& MemberVariable) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Self) {
			MemberVariable = Cast<OwnerType>(Self->GetOwner());
		}

		return MemberVariable;
	}

	// Lazily get component and run init callback if needed.
	template<typename CompType, typename InitCallback>
	static CompType* LazyGetCompWithInit(AActor* Owner, CompType*& MemberVariable, InitCallback Callback){
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Owner) {
			MemberVariable = LazyGetComp(Owner, MemberVariable);

			if (MemberVariable) {
				Callback(MemberVariable);
			}
		}

		return MemberVariable;
	};

	// Lazily get owner and run callback on the retrieved owner.
	template<typename OwnerType, typename InitCallback>
	static OwnerType* LazyGetOwnerWithInit(UActorComponent* Self, OwnerType*& MemberVariable, InitCallback Callback) {
		if (MemberVariable) {
			return MemberVariable;
		}

		if (Self) {
			MemberVariable = Cast<OwnerType>(Self->GetOwner());
		}

		if (MemberVariable) {
			Callback(MemberVariable);
		}

		return MemberVariable;
	}

	// Perform a callback when a value enters / exits a deadzone. Entering a deadzone is a less typical case so it is optional.
	template<typename F1, typename F2>
	static void DeadzoneAction(float CurrentAxis, float PreviousAxis, float deadzone, F1 ExitDeadzoneCallback, F2 EnterDeadzoneCallback) {
		bool PreviouslyInDeadzone = UKismetMathLibrary::InRange_FloatFloat(PreviousAxis, deadzone*-1.f, deadzone);
		bool CurrentlyInDeadzone = UKismetMathLibrary::InRange_FloatFloat(CurrentAxis, deadzone*-1.f, deadzone);
		if (PreviouslyInDeadzone != CurrentlyInDeadzone) {
			if (CurrentlyInDeadzone) {
				EnterDeadzoneCallback();
			} else {
				ExitDeadzoneCallback();
			}
		}
	}

	// Attempt to get an existing item in a TMap cache.
	template<typename CompType, typename OutputType = CompType>
	static OutputType* GetCachedComponentByTag(AActor* Ctx, const FString& Key, TMap<FString, OutputType*>& Cache) {

		// Get Existing Cached Component
		auto FoundComp = Cache.Find(Key);
		if (FoundComp) {
			return *FoundComp;
		}

		// Search for component to Cache
		auto Comps = Ctx->GetComponentsByTag(CompType::StaticClass(), FName(*Key));
		for (auto i = Comps.CreateIterator(); i; ++i) {
			auto Comp = Cast<OutputType>(*i);
			if (Comp) {
				Cache.Add(Key, Comp);
				return Comp;
			}
		}

		// Failure Case
		return nullptr;
	};

	class Math {
		public:
		static int32 Mod(int32 a, int32 b) {
			int32 Out = a % b;
			if (Out >= 0) {
				return Out;
			}
			return Out + b;
		}
	};

	class Network {
		public:
			// Runs callback only for Client.
			template<typename Callback>
			static void Client(APawn* Ctx, Callback Foo) {
				if (Ctx && !(Ctx->GetWorld()->IsServer())) {
					Foo();
				}
			}

			// Runs callback only for Server.
			template<typename Callback>
			static void Server(APawn* Ctx, Callback Foo) {
				if (Ctx && !(Ctx->GetWorld()->IsServer())) {
					Foo();
				}
			}

			// Runs only for Local Context (Locally Controlled), may execute on specific client or server.
			template<typename Callback>
			static void Local(APawn* Ctx, Callback Foo) {
				if (Ctx && (Ctx->IsLocallyControlled())) {
					Foo();
				}
			}

			// Shorthand function for mapping to multiple Context Types
			template<typename Callback>
			static void Execute(APawn* Ctx, Callback Foo, bool Client, bool Server, bool Local) {
				if (Client) {
					Client(Ctx, Foo);
				}
				if (Server) {
					Server(Ctx, Foo);
				}
				if (Local) {
					Local(Ctx, Foo);
				}
			}
	};

	class JSON {
		public:
		static FString ToString(const FJsonObject& Object) {
			// Create heap instance for Shared Pointer through Copy Constructor
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject(Object));

			// Write Object to String
			FString OutputString;
			TSharedRef<TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
			
			return OutputString;
		}
		static bool ToObject(const FString& String, FJsonObject& Out) {
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(String);
			TSharedPtr<FJsonObject> JsonParsed;
			UE_LOG(LogTemp, Warning, TEXT("ToObject Inner"));
			if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
				UE_LOG(LogTemp, Warning, TEXT("ToObject Deserialize Succeeded"));
				if (JsonParsed.IsValid()) {
					UE_LOG(LogTemp, Warning, TEXT("ToObject Json is Valid"));
					Out = *(JsonParsed.Get());
					return true;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("ToObject Json is Invalid"));
			return false;
		}
	};

	/* Utilities that operate so long as an enum fulfills the PCPP_UE4 enum interface.
		All that it required is for the last member of the enum to be "MAX"
	*/
	class EnumMap {
		template<typename EnumType,typename ValueType>
		static void Init(TArray<ValueType>& Map) {
			Map.ResizeTo((int32)EnumType::Max);
		}

		template<typename EnumType, typename ValueType>
		static ValueType Get(EnumType Key, TArray<ValueType>& Map) {
			return Map[(int32)Key];
		}

		template<typename EnumType, typename ValueType>
		static void Set(EnumType Key, ValueType Value, TArray<ValueType>& Map) {
			Map[(int32)Key] = Value;
		}
	};
};
