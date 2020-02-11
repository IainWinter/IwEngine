#pragma once

#include "iw/reflection/Reflect.h"

enum REFLECT EnemyType {
	SPIN
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
