#pragma once

#include "Weapons.h"
#include "iw/engine/Components/Timer.h"
#include <array>

struct Move;

struct Player
{
	float i_moveX = 0.f;
	float i_moveY = 0.f;
	glm::vec3 move; // normed vector of i_moves

	// fire
	bool  i_fire1 = false;
	bool  i_fire2 = false;

	float u_speed = 200;           // max speed
	//float u_acceleration = 300;  // accelerate to max speed
	//float u_impulse = 200;       // initial speed on button press

	//float rotation = 0.f;

	bool can_fire_laser = false;

	Weapon* CurrentWeapon = nullptr;
	Weapon* SpecialLaser = nullptr;

	std::array<bool,  4> Use;
	std::array<Move*, 4> Moves; // limit moves to 4

	bool NoDamage = false;

	~Player()
	{
		for (Move* move : Moves)
		{
			delete move;
		}
	}

	//glm::vec3 imp;
	//glm::vec3 vel;

	// returns the new vel

	//float CalcImp(float move)
	//{
	//	return move * u_impulse;
	//}

	//float CalcAcc(float vel, float nvel, float move, float dt)
	//{
	//	if (move != 0.f)
	//	{
	//		vel += move * u_acceleration * dt;
	//	}

	//	else 
	//	{
	//		int sign = signbit(vel);
	//		vel -= nvel * u_acceleration /*u_breaking*/ * dt;
	//		if (sign != signbit(vel))
	//		{
	//			vel = 0;
	//		}
	//	}

	//	return vel;
	//}

	//float CalcVel(float vel, float nvel, float move, float dt)
	//{
	//	return /*CalcImp(move) + */CalcAcc(vel, nvel, move, dt);
	//}
};
