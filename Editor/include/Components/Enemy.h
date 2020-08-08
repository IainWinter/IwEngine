#pragma once

#include "Bullet.h"
#include <vector>

enum class REFLECT EnemyType {
	SPIN               = 0x0,
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

	float Timer;
	bool  HasShot;

	// only for boss
	float Timer2;

	// only for circle might get removed
	float RotSpeed;

	// only really for bosses
	int Health;
	int ScoreMultiple;

	bool JustHit;
};

struct Action {
	unsigned Index;
	float Time;
	float Delay;

	int Condition = -1;
};

struct EnemyBoss {
	std::vector<Action> Actions;

	float ActionTimer = 0.0f;
	int CurrentAction = -1;
};
