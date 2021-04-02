#pragma once

#include "Bullet.h"
#include <vector>

enum class REFLECT EnemyType {
	SPIN             = 0x0,
	CIRCLE,
	SEEK,
	MINI_BOSS_FOREST = 0x10,
	MINI_BOSS_CANYON = 0x11,
	BOSS_FOREST      = 0x100
};

struct REFLECT Enemy {
	REFLECT EnemyType Type;
	REFLECT Bullet Bullet;
	REFLECT float Speed;
	REFLECT float FireTime;
	REFLECT float ChargeTime;
	REFLECT float Rotation;

	float Timer = 0;
	bool  HasShot = false;

	// only for boss
	float Timer2 = 0;

	// only for circle might get removed
	float RotSpeed = 0;

	// only really for bosses
	int Health = 0;
	int ScoreMultiple = 0;

	bool JustHit = 0;
};

struct Action {
	unsigned Index;
	float Time;
	float Delay;

	int Condition = -1;
	int EveryTime = -1; // -1 no; 1 enabled; 0 disabled
};

struct EnemyBoss {
	std::vector<Action> Actions;

	float ActionTimer = 0.0f;
	int FirstAction = -1;
	int CurrentAction = -1;

	bool JustSpawned = true;
	iw::vector3 Target;
};

struct RaftEnemy {
	unsigned PathIndex = 0;
	unsigned IntraPathIndex = 0;

	float Timer = 0.0f;
	float Speed = 5.0f;
};
