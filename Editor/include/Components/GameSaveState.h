#pragma once

#include "iw/reflection/Reflect.h"

struct REFLECT GameSaveState {
	REFLECT bool Cave03LadderDown = false;
	REFLECT bool Cave06LadderDown = false;
	REFLECT bool Top08LadderDown  = false;
	REFLECT bool Canyon03BossKilled = false;
};
