#pragma once

#include "Enemy.h"
#include "iw/math/vector3.h"
#include "iw/graphics/Model.h"
#include <vector>

struct Level {
	std::vector<Enemy> Enemies;
	std::vector<iw::vector3> Positions;
	IW::Model level;
};
