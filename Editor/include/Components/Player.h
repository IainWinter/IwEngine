#pragma once

#include "iw/reflection/Reflect.h"

struct REFLECT Player {
	REFLECT float Speed;
	REFLECT float DashTime;
	REFLECT float CooldownTime;
	REFLECT int   Health;

	float Timer;
	float DeathTimer;
	bool  Damaged;
};
