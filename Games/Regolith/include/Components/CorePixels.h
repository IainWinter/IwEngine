#pragma once

#include <unordered_set>

struct CorePixels
{
	float TimeWithoutCore = 0.f; // time to death after half of core is gone
	float Timer = 0.f;

	int ActiveCount = 0;
	std::unordered_set<int> Indices;

	int CenterX, CenterY;

	bool ScannedTile = false;

	CorePixels() {}
};
