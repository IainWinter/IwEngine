#pragma once

#include"glm/vec2.hpp"

struct Player {
	bool i_up; // inputs - get set in event loop
	bool i_down;
	bool i_left;
	bool i_right;
	bool i_fire1;
	bool i_fire2;

	float FireTimeout = 0;

	//int BulletAmmo    = 50; // 50 rounds max, recharge when not firing
	//int MaxBulletAmmo = 50;

	int MissileAmmo    = 46; // 24 missiles max, recharge by mining rez
	int MaxMissileAmmo = 46;

	float Energy = 100;    // 1 second of boost is 10 energy, 1 second of mLASER is 33 energy
	float MaxEnergy = 100;
};
