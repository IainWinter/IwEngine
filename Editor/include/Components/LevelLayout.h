#pragma once

#include "iw/reflection/Reflect.h"
#include "iw/math/vector3.h"
#include <string>
#include <vector>

struct REFLECT LevelLayout {
	REFLECT std::string LevelName;
	REFLECT std::vector<LevelLayout> Connections;

	LevelLayout* Next(
		unsigned i = 0)
	{
		auto itr = Connections.begin() + i;
		if (itr >= Connections.end()) {
			return nullptr;
		}

		return &*itr;
	}

	void AddConnection(
		LevelLayout& layout)
	{
		Connections.push_back(layout);
	}
};
