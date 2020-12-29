#pragma once

#include "iw/common/Components/Transform.h"
#include <vector>

struct Tile {
	std::vector<std::pair<int, int>> Positions;
	int X = 0;
	int Y = 0;
};
