#pragma once

#include "iw/reflection/Reflect.h"
#include <unordered_map>

struct REFLECT GameSaveState {
	// Inter run state
	REFLECT bool Cave03LadderDown = false;
	REFLECT bool Cave06LadderDown = false;
	REFLECT bool Top08LadderDown  = false;
	REFLECT bool Canyon03BossKilled = false;

	// Intra run state

	std::unordered_map<std::string, bool> IntraState;

	bool GetState(
		std::string state)
	{
		auto itr = IntraState.find(state);
		if (itr == IntraState.end()) {
			itr = IntraState.emplace(state, false).first;
		}

		return itr->second;
	}

	void SetState(
		std::string state,
		bool value)
	{
		auto itr = IntraState.find(state);
		if (itr != IntraState.end()) {
			itr->second = value;
		}
	}
};
