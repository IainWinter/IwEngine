#pragma once

#include "Components/BulletPackage.h"
#include "iw/math/vector3.h"

struct REFLECT Bullet {
	REFLECT BulletType  Type;
	REFLECT float       Speed;

	REFLECT PackageType Package;

	float       Timer;
	iw::vector3 InitialVelocity;
};
