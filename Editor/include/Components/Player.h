#pragma once

#include "iw/reflection/Reflect.h"

struct REFLECT Player {
	REFLECT float Speed;
	REFLECT float DashTime;
	REFLECT float ChargeTime;
	REFLECT int   Health;

	float Timer;
	float DeathTimer;
	bool  Damaged;

	// just took form system

	iw::vector3 Movement;

	bool Left, Right, Up, Down, Dash, Sprint, Transition, LevelTransition;

	iw::vector3 TransitionStartPosition;
	iw::vector3 TransitionTargetPosition;

	float Begin;
	float TransitionSpeed;
};
