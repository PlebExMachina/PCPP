#pragma once
#include "Templates/UnrealTemplate.h"


/*
* Contains utilities for interacting with groups of array iterators.
* Syncrhonizing multiple arrays is a common use case for Data Oriented Design purposes.
*/
namespace PCPP_Iterator {
	// Generates a pack of Iterators for concise function calls.
	template<typename ... Ts>
	auto GenerateIteratorPack(Ts &... xs) {
		return xs;
	}

	void Increment() {}
	// Advances Every Iterator Present
	template<typename T, typename ... Ts>
	void Increment(T& x, Ts &... xs) {
		++x;
		PCPP_Iterator::Increment(xs...);
	}

	void RemoveCurrent() {}
	// Removes the Current Array Entry of every Iterator Present and advances them.
	template<typename T, typename ... Ts>
	void RemoveCurrent(T& x, Ts&... xs) {
		x.RemoveCurrent();
		PCPP_Iterator::RemoveCurrent(xs...);
	}

	// Removes Entry if condition is True, otherwise advances the iterator.
	template<typename ... Ts>
	void RemoveConditional(bool Condition, Ts&... xs) {
		if (Condition) {
			PCPP_Iterator::RemoveCurrent(xs...);
		} else {
			PCPP_Iterator::Increment(xs...);
		}
	}


}
