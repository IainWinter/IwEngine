#pragma once

#include "Bullet.h"

enum class REFLECT EnemyType {
	SPIN,
	CIRCLE
};

struct REFLECT Enemy {
	REFLECT EnemyType Type;
	REFLECT Bullet Bullet;
	REFLECT float Speed;
	REFLECT float FireTime;
	REFLECT float CooldownTime;

	float Timer;
	float Rotation;
	bool  HasShot;
};
