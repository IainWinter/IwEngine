#pragma once

#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"
#include "plugins/iw/Sand/Defs.h"
#include "Components/Player.h"
#include "Components/Item.h"
#include "Components/Projectile.h"
#include "Components/Weapon.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
	SPAWN_ITEM,
	CHANGE_LASER_FLUID,
	HEAL_PLAYER,
	PROJ_HIT_TILE,
	CHANGE_WEAPON,
	FIRE_WEAPON
};

struct SpawnEnemy_Event : iw::SingleEvent
{
	iw::Entity ShootAt;
	float SpawnLocationX;
	float SpawnLocationY;
	float TargetLocationX;
	float TargetLocationY;

	SpawnEnemy_Event(
		iw::Entity shootAt,
		float spawnLocationX,
		float spawnLocationY,
		float targetLocationX,
		float targetLocationY
	)
		: iw::SingleEvent(SPAWN_ENEMY)
		, ShootAt(shootAt)
		, SpawnLocationX(spawnLocationX)
		, SpawnLocationY(spawnLocationY)
		, TargetLocationX(targetLocationX)
		, TargetLocationY(targetLocationY)
	{}
};

struct SpawnProjectile_Event : iw::SingleEvent
{
	ShotInfo Shot;
	int Depth;

	SpawnProjectile_Event(
		const ShotInfo& shot,
		int depth = 0
	)
		: iw::SingleEvent(SPAWN_PROJECTILE)
		, Shot(shot)
		, Depth(depth)
	{}
};

struct SpawnItem_Event : iw::SingleEvent
{
	float X, Y;
	int Amount;

	ItemType Item;

	SpawnItem_Event(
		float x, float y,
		int amount,
		ItemType item
	)
		: iw::SingleEvent(SPAWN_ITEM)
		, X(x)
		, Y(y)
		, Amount(amount)
		, Item(item)
	{}
};

struct ChangeLaserFluid_Event : iw::SingleEvent
{
	int Amount;

	ChangeLaserFluid_Event(
		int amount
	)
		: iw::SingleEvent(CHANGE_LASER_FLUID)
		, Amount(amount)
	{}
};

struct HealPlayer_Event : iw::SingleEvent
{
	HealPlayer_Event()
		: iw::SingleEvent(HEAL_PLAYER)
	{}
};

struct ProjHitTile_Event : iw::SingleEvent
{
	iw::TileInfo Info;
	iw::Entity Hit;
	iw::Entity Projectile;
	
	ProjHitTile_Event(
		iw::TileInfo info,
		iw::Entity hit,
		iw::Entity proj
	)
		: iw::SingleEvent(PROJ_HIT_TILE)
		, Info(info)
		, Hit(hit)
		, Projectile(proj)
	{}
};

struct ChangeWeapon_Event : iw::SingleEvent
{
	iw::Entity Entity;
	WeaponType Weapon;
	int        WeaponAmmo;

	ChangeWeapon_Event(
		iw::Entity entity,
		WeaponType weapon,
		int weaponAmmo = -1 // infinite
	)
		: iw::SingleEvent(CHANGE_WEAPON)
		, Entity(entity)
		, Weapon(weapon)
		, WeaponAmmo(weaponAmmo)
	{}
};

struct FireWeapon_Event : iw::SingleEvent
{
	iw::Entity Entity;
	float TargetX;
	float TargetY;

	FireWeapon_Event(
		iw::Entity entity,
		float targetX,
		float targetY
	)
		: iw::SingleEvent(FIRE_WEAPON)
		, Entity(entity)
		, TargetX(targetX)
		, TargetY(targetY)
	{}
};
