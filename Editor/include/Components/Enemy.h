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
	REFLECT float Rotation;

	float Timer;
	bool  HasShot;

	// only for circle might get removed
	float RotSpeed;
};
