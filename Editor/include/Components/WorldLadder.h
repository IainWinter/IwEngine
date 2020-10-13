#pragma once

#include <string>

struct WorldLadder {
	std::string Level = "";
	bool Open  = false;

	bool Above = false;
	bool Transition = false;
	float Speed = 1;
	bool* SaveState = nullptr;
};
