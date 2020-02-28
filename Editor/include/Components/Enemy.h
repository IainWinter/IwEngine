#pragma once

#include "iw/reflection/Reflect.h"

enum class REFLECT EnemyType {
	SPIN,
	CIRCLE
};

struct REFLECT Enemy {
	REFLECT EnemyType Type;
	REFLECT float Speed;
	REFLECT float FireTime;
	REFLECT float CooldownTime;

	float Timer;
	float Rotation;
	bool  HasShot;
};
