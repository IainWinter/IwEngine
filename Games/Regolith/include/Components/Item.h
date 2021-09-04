#pragma once

#include <functional>

enum ItemType {
	HEALTH,
	LASER_CHARGE,
	WEAPON_MINIGUN,
	PLAYER_CORE
};

struct Item {
	float MoveTime = 2.f;
	float MoveTimer = 0.f;

	float LifeTimer = 5.f;
	bool DieWithTime = true;

	float ActivateTimer = 0.f;

	float PickupTimer = 1.f;
	float PickUpRadius = 100.f;
	bool PickingUp = false;
	std::function<void()> OnPickUp;
};
