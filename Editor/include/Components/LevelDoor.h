#pragma once

#include "iw/reflection/Reflect.h"
#include "iw/math/vector3.h"
#include <string>

enum class REFLECT LevelDoorState {
	OPEN,
	LOCKED
};

struct REFLECT LevelDoor {
	REFLECT LevelDoorState State;

	REFLECT int Index;
	REFLECT iw::vector3 InPosition;

	float ColorTimer;
};
