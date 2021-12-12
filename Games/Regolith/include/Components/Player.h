#pragma once

#include "iw/engine/Components/Timer.h"
#include "Components/Weapon.h"

struct Player
{
	float i_moveX = 0.f;
	float i_moveY = 0.f;
	bool  i_fire1 = false;
	bool  i_fire2 = false;

	float u_speed = 200;
	float u_acceleration = 300;
	float u_breaking = 150;

	float rotation = 0.f;

	bool can_fire_laser = false;

	Weapon* CurrentWeapon = nullptr;
	Weapon* SpecialLaser = nullptr;

	float CalcVel(float vel, float nvel, float move, float dt)
	{
		if (move != 0.f)
		{
			vel += move * u_acceleration * dt;
		}

		else 
		{
			int sign = signbit(vel);
			vel -= nvel * u_breaking * iw::FixedTime();
			if (sign != signbit(vel))
			{
				vel = 0;
			}
		}

		return vel;
	}
};
