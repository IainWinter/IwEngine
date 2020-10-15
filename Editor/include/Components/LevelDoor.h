#pragma once

#include "iw/reflection/Reflect.h"
#include "iw/math/vector3.h"
#include <string>

enum class REFLECT LevelDoorState {
	OPEN,
	LOCKED
};

struct REFLECT LevelDoor {
	REFLECT bool GoBack;
	REFLECT LevelDoorState State;
	REFLECT std::string NextLevel;

	float ColorTimer;

	int Index = 0;
};
