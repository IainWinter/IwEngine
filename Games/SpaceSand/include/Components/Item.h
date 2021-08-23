#pragma once

#include <functional>

enum ItemType {
	HEALTH,
	LASER_CHARGE,
	WEAPON_MINIGUN,
};

struct Item {
	float MoveTime = 2.f;
	float MoveTimer = 0.f;

	float PickUpRadius = 100.f;
	bool PickingUp = false;
	std::function<void()> OnPickUp;
};
