#pragma once

#include "Bullet.h"

enum class REFLECT EnemyType {
	SPIN,
	CIRCLE,
	SEEK
};

struct REFLECT Enemy {
	REFLECT EnemyType Type;
	REFLECT Bullet Bullet;
	REFLECT float Speed;
	REFLECT float FireTime;
	REFLECT float ChargeTime;

	float Timer;
	float Rotation;
	bool  HasShot;
};
