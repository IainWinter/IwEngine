#pragma once

#include <functional>

enum class ItemType {
	NONE,

	HEALTH,
	LASER_CHARGE,
	PLAYER_CORE,
	WEAPON_MINIGUN,
	WEAPON_BOLTZ,
	WEAPON_WATTZ,
};

struct Item {
	ItemType Type = ItemType::NONE;

	float MoveTime = 2.f;
	float MoveTimer = 0.f;

	float LifeTimer = 5.f;
	bool DieWithTime = true;

	float ActivateTimer = 0.f;

	float PickupTimer = 1.f;
	float PickUpRadius = 100.f;
	bool PickingUp = false;
	std::function<void()> OnPickUp;

	Item() {}
};
