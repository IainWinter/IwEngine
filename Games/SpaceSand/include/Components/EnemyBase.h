#pragma once

#include "iw/entity/EntityHandle.h"
#include "glm/vec3.hpp"
#include <vector>

struct EnemyBase {
	EnemyBase* MainBase = nullptr; // Null if main base

	std::vector<iw::EntityHandle> NeedsObjective;

	glm::vec3 EstPlayerLocation = glm::vec3(0, 0, FLT_MAX); // x, y, radius

	int Rez = 1000; // Start with ability to spawn 10 ships

	int AttackShipCost = 100;
	int SupplyShipCost = 500;

	float FireTimer = 0;
	float FireTime = 10;

	float UseRezTimer = 0;
	float UseRezTime = 5;

	void RequestObjective(iw::EntityHandle ship) {
		NeedsObjective.push_back(ship);
	}
};
