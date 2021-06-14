#pragma once

#include "Attack.h"

#include "iw/engine/Events/ActionEvents.h"

#include <functional>

enum class myActionEvents {
	MOVE,
	ATTACK
};

struct event_Move : iw::SingleEvent {
	float x, y;
	bool jump;

	event_Move(
		float x, float y,
		bool jump
	)
		: iw::SingleEvent(iw::val(myActionEvents::MOVE))
		, x(x)
		, y(y)
		, jump(jump)
	{}
};

struct event_Attack : iw::SingleEvent {
	AttackType Type;
	iw::EntityHandle Entity; // Expected to have transform
	float Facing;

	// To be assigned through event path
	AttackProps Props;

	event_Attack(
		AttackType type,
		iw::EntityHandle entity,
		float facing
	)
		: iw::SingleEvent(iw::val(myActionEvents::ATTACK))
		, Type(type)
		, Entity(entity)
		, Facing(facing)
	{}
};
