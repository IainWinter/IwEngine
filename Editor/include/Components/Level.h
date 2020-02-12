#pragma once

#include "Enemy.h"
#include "iw/math/vector2.h"
//#include "iw/graphics/Model.h"
#include <vector>

struct REFLECT Level {
	REFLECT std::vector<Enemy>       Enemies;
	REFLECT std::vector<iw::vector2> Positions;
	REFLECT std::string StageName;
	//iw::Model Stage;
};
