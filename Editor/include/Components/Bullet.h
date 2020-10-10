#pragma once

#include "Components/BulletPackage.h"
#include "iw/math/vector3.h"

struct REFLECT Bullet {
	REFLECT BulletType Type  = BulletType::LINE;
	REFLECT float      Speed = 0;

	REFLECT PackageType Package = PackageType::NONE;

	float       Timer = 0.0f;
	iw::vector3 InitialVelocity;
};
