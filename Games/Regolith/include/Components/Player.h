#pragma once

#include "iw/engine/Components/Timer.h"
#include "Components/Weapon.h"

struct Player
{
	float i_moveX;
	float i_moveY;
	bool  i_fire1;
	bool  i_fire2;

	float speed = 150;

	float rotation;

	bool can_fire_laser;

	Weapon* CurrentWeapon;
	Weapon* SpecialLaser;
};
