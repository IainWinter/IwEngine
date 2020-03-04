#pragma once

#include "Enemy.h"
#include "iw/math/vector2.h"
//#include "iw/graphics/Model.h"
#include "Player.h"
#include "LevelDoor.h"
#include <vector>

struct REFLECT Level {
	REFLECT std::vector<Enemy>       Enemies;
	REFLECT std::vector<iw::vector2> Positions;
	REFLECT std::string StageName = "";
	REFLECT Player Player = {};
	REFLECT LevelDoor Door = {};
	//iw::Model Stage;
};
