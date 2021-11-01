#pragma once

#include "iw/engine/Components/Timer.h"
#include "Components/Weapon.h"

struct Player
{
	float i_moveX = 0.f;
	float i_moveY = 0.f;
	bool  i_fire1 = false;
	bool  i_fire2 = false;

	float speed = 150;

	float rotation = 0.f;

	bool can_fire_laser = false;

	Weapon* CurrentWeapon = nullptr;
	Weapon* SpecialLaser = nullptr;
};
