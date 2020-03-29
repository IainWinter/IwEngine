#pragma once

#include "iw/reflection/Reflect.h"
#include "iw/math/vector3.h"

enum BulletType {
	LINE,
	SINE,
	ORBIT,
	SEEK
};

struct REFLECT Bullet {
	REFLECT BulletType Type;
	REFLECT float      Speed;

	float Time;
	iw::vector3 initialVelocity;
	int enemyIndex;
};
