#pragma once

#include <unordered_set>

struct CorePixels
{
	float TimeWithoutCore = 0.f; // time to death after half of core is gone
	float Timer = 0.f;

	std::unordered_set<int> Indices;
	std::unordered_set<int> ActiveIndices;

	int CenterX = 0;
	int CenterY = 0;

	bool ScannedTile = false;

	CorePixels() {}
};
