#pragma once

#include "iw/entity/Entity.h"
#include "iw/engine/Components/Timer.h"

struct EnemyShip {
	iw::Entity ShootAt;
	iw::Timer Timer;

	EnemyShip() {}
};
