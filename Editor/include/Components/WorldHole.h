#pragma once

#include <string>

struct WorldHole {
	bool InTransition = false;

	bool Touched = false;
	std::string CaveLevel;

	float Timer = 0.0f;
	float Time;

	float BulletTimer = 0.0f;
	float BulletTime;
};
