#pragma once

#include "iw/entity/Entity.h"
#include "Components/Weapon.h"

struct EnemyShip {
	iw::Entity ShootAt;
	Weapon* Weapon;
};
