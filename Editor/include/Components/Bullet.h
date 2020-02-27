#pragma once

#include "iw/reflection/Reflect.h"

enum BulletType {
	LINE
};

struct REFLECT Bullet {
	REFLECT BulletType Type;
	REFLECT float      Speed;
	REFLECT float      Time;
};
