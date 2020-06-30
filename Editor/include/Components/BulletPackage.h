#pragma once

#include "iw/reflection/Reflect.h"

enum BulletType : unsigned {
	LINE,  // Moves in line with constant velocity
	SINE,  // Moves in line with velocity on a sine wave
	ORBIT, // Adds a force towards a target, causing an orbit
	SEEK   // Moves towards a target with constant velocity
};

enum PackageType : unsigned {
	TIMER    = 1 << (sizeof(PackageType) * 4 + 0), // Explodes on a timer
	DISTANCE = 1 << (sizeof(PackageType) * 4 + 1), // Explosed when close to a target


	REMOVE_TYPE = 0b00000000000000001111111111111111
};

struct REFLECT BulletPackage {
	REFLECT BulletType InnerType;
	REFLECT float      InnerSpeed;
	REFLECT float      TimeToExplode = 3.0f; // no way to set this rn

	bool Exploded = false;
};
