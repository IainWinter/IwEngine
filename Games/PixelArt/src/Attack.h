#pragma once

#include "iw/physics/Collision/HullCollider.h"
#include "iw/entity/Entity.h"

enum class AttackType {
	GROUND_LIGHT_FORWARD,
	GROUND_LIGHT_UP,
	GROUND_LIGHT_DOWN,

	GROUND_HEAVY_FORWARD,
	GROUND_HEAVY_UP,
	GROUND_HEAVY_DOWN,

	AIR_LIGHT_FORWARD,
	AIR_LIGHT_UP,
	AIR_LIGHT_DOWN,

	AIR_HEAVY_FORWARD,
	AIR_HEAVY_UP,
	AIR_HEAVY_DOWN,

	FORWARD_SPECIAL,
	UP_SPECIAL,
	DOWN_SPECIAL,

	LENGTH
};

struct AttackProps {
	iw::Transform Transform;

	iw::Hull2 Hitbox;
	glm::vec2 Knockback = glm::vec2(0);
	glm::vec2 Velocity  = glm::vec2(0);

	float Time = .1f;

	std::function<bool(iw::EntityHandle)> func_deleteIf;
};

struct Attack {
	iw::EntityHandle Source;
};
