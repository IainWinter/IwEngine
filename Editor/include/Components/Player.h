#pragma once

#include "iw/reflection/Reflect.h"

struct REFLECT Player {
	REFLECT float Speed;
	REFLECT float DashTime;
	REFLECT float ChargeTime;
	REFLECT int   Health;

	float Timer = 0;
	float DeathTimer = 0;
	bool  Damaged = false;

	// just took form system

	iw::vector3 Movement = 0;

	bool Left, Right, Up, Down, Dash, Sprint, Transition;

	iw::vector3 TransitionStartPosition = 0;
	iw::vector3 TransitionTargetPosition = 0;

	float Begin = 0;
	float TransitionSpeed = 0;
};
