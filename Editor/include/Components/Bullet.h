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

	float Timer;
	iw::vector3 initialVelocity;
};

struct BulletPackage {
	REFLECT BulletType InnerType;
	REFLECT float      InnerSpeed;

	float Time;
};
