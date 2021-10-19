#pragma once

#include "iw/entity/Entity.h"
#include "iw/engine/Components/Timer.h"
#include "Components/Weapon.h"

enum EnemyType
{
	FIGHTER,
	BOMB,
	STATION,
	BASE
};

struct Enemy
{
	iw::Entity Target;
	float ExplosionPower;

	float ActiveTimer = 0.f;
	float ActiveTime  = .1f;
};

struct Fighter_Enemy {
	Weapon* Weapon;
};

struct Bomb_Enemy {
	float TimeToExplode;
	float RadiusToExplode;
	bool Explode;
};

struct Station_Enemy {
	iw::Timer timer;

	Station_Enemy() {}
};

struct Base_Enemy {

};
