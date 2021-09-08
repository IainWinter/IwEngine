#pragma once

#include "iw/entity/Entity.h"
#include "Components/Weapon.h"

enum EnemyType
{
	FIGHTER,
	BOMB,
	STATION
};

struct Enemy
{
	iw::Entity Target;
};

struct Fighter_Enemy {
	Weapon* Weapon;
};

struct Bomb_Enemy {
	float TimeToExplode;
	float RadiusToExplode;
	float ExplosionPower;

	bool Explode;
};

struct Station_Enemy {

};
