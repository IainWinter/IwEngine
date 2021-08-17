#pragma once

#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
	SPAWN_ITEM,
};

struct SpawnEnemy_Event : iw::SingleEvent
{
	iw::Entity ShootAt;
	float TargetLocationX;
	float TargetLocationY;

	SpawnEnemy_Event(
		iw::Entity shootAt,
		float targetLocationX,
		float targetLocationY
	)
		: iw::SingleEvent(SPAWN_ENEMY)
		, ShootAt(shootAt)
		, TargetLocationX(targetLocationX)
		, TargetLocationY(targetLocationY)
	{}
};

struct SpawnProjectile_Event : iw::SingleEvent
{
	float X, Y, dX, dY;

	enum PType {
		BULLET,
		LASER
	} Type;

	int Depth;

	SpawnProjectile_Event(
		float x,  float y,
		float dx, float dy,
		PType type,
		int depth = 0
	)
		: iw::SingleEvent(SPAWN_PROJECTILE)
		, X(x)
		, Y(y)
		, dX(dx)
		, dY(dy)
		, Type(type)
		, Depth(depth)
	{}
};

struct SpawnItem_Event : iw::SingleEvent
{
	float X, Y;
	int Amount;

	enum IType {
		HEALTH,
		LASER_CHARGE
	} Type;

	SpawnItem_Event(
		float x, float y,
		int amount,
		IType type
	)
		: iw::SingleEvent(SPAWN_ITEM)
		, X(x)
		, Y(y)
		, Amount(amount)
		, Type(type)
	{}
};
