#pragma once

#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"
#include "plugins/iw/Sand/Defs.h"
#include "Components/Player.h"
#include "Components/Item.h"
#include "Components/ProjHead.h"
#include "Components/EnemyShip.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
	SPAWN_ITEM,
	SPAWN_ASTEROID,
	SPAWN_EXPLOSION,
	CHANGE_LASER_FLUID,
	HEAL_PLAYER,
	CHANGE_PLAYER_WEAPON,
	PROJ_HIT_TILE,
	REMOVE_CELL_FROM_TILE,
	CREATED_PLAYER,
	CREATED_CORE_TILE,
	CORE_EXPLODED,
	STATE_CHANGE,
	OPEN_MENU,
};

struct SpawnExplosion_Config
{
	float SpawnLocationX = 0;
	float SpawnLocationY = 0;
	float ExplosionRadius = 10;
	float ExplosionPower = 200;
};

struct SpawnExplosion_Event : iw::SingleEvent
{
	SpawnExplosion_Config Config;

	SpawnExplosion_Event(
		SpawnExplosion_Config& config
	)
		: iw::SingleEvent(SPAWN_EXPLOSION)
		, Config(config)
	{}
};

struct SpawnEnemy_Config
{
	iw::Entity TargetEntity;
	float SpawnLocationX;
	float SpawnLocationY;
	float TargetLocationX;
	float TargetLocationY;

	EnemyType EnemyType;
};

struct SpawnEnemy_Event : iw::SingleEvent
{
	SpawnEnemy_Config Config;

	SpawnEnemy_Event(
		SpawnEnemy_Config config
	)
		: iw::SingleEvent(SPAWN_ENEMY)
		, Config(config)
	{}
};

struct SpawnAsteroid_Config
{
	float SpawnLocationX = 0;
	float SpawnLocationY = 0;
	float VelocityX = 0;
	float VelocityY = 0;
	float AngularVel = 0;
	int Size = 1;
};

struct SpawnAsteroid_Event : iw::SingleEvent
{
	SpawnAsteroid_Config Config;

	SpawnAsteroid_Event(
		const SpawnAsteroid_Config& config
	)
		: iw::SingleEvent(SPAWN_ASTEROID)
		, Config(config)
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

struct SpawnItem_Config
{
	float X = 0, Y = 0;
	int Amount = 1;

	float ActivateDelay = 0.f;
	float Speed = 75;
	float AngularSpeed = 0;

	bool DieWithTime = true;

	ItemType Item;
	std::function<void()> OnPickup;
};

struct SpawnItem_Event : iw::SingleEvent
{
	SpawnItem_Config Config;

	SpawnItem_Event(
		const SpawnItem_Config& config
	)
		: iw::SingleEvent(SPAWN_ITEM)
		, Config(config)
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
	bool IsCore;

	HealPlayer_Event(
		int isCore = false
	)
		: iw::SingleEvent(HEAL_PLAYER)
		, IsCore(isCore)
	{}
};

struct ChangePlayerWeapon_Event : iw::SingleEvent
{
	WeaponType Weapon;

	ChangePlayerWeapon_Event(
		WeaponType weapon
	)
		: iw::SingleEvent(CHANGE_PLAYER_WEAPON)
		, Weapon(weapon)
	{}
};

struct ProjHitTile_Config
{
	int X;
	int Y;
	iw::TileInfo Info;
	iw::Entity Hit;
	iw::Entity Projectile;
};

struct ProjHitTile_Event : iw::SingleEvent
{
	ProjHitTile_Config Config;
	
	ProjHitTile_Event(
		const ProjHitTile_Config& config
	)
		: iw::SingleEvent(PROJ_HIT_TILE)
		, Config(config)
	{}
};

struct RemoveCellFromTile_Event : iw::SingleEvent
{
	size_t Index;
	iw::Entity Entity;

	RemoveCellFromTile_Event(
		size_t index, iw::Entity entity
	)
		: iw::SingleEvent(REMOVE_CELL_FROM_TILE)
		, Index(index)
		, Entity(entity)
	{}
};

struct CreatedPlayer_Event : iw::SingleEvent
{
	iw::Entity PlayerEntity;

	CreatedPlayer_Event(
		iw::Entity playerEntity
	)
		: iw::SingleEvent(CREATED_PLAYER)
		, PlayerEntity(playerEntity)
	{}
};

struct CreatedCoreTile_Event : iw::SingleEvent
{
	iw::Entity TileEntity;

	CreatedCoreTile_Event(
		iw::Entity tileEntity
	)
		: iw::SingleEvent(CREATED_CORE_TILE)
		, TileEntity(tileEntity)
	{}
};


struct CoreExploded_Event : iw::SingleEvent
{
	iw::Entity Entity;

	CoreExploded_Event(
		iw::Entity entity
	)
		: iw::SingleEvent(CORE_EXPLODED)
		, Entity(entity)
	{}
};

enum StateName {
	END_STATE,
	RUN_STATE,
	PAUSE_STATE,
	RESUME_STATE,
};

struct StateChange_Event : iw::SingleEvent
{
	StateName State;

	StateChange_Event(
		StateName state
	)
		: iw::SingleEvent(STATE_CHANGE)
		, State(state)
	{}
};

enum MenuName {
	ECS_MENU
};

struct OpenMenu_Event : iw::SingleEvent
{
	MenuName Menu;

	OpenMenu_Event(
		MenuName menu
	)
		: iw::SingleEvent(OPEN_MENU)
		, Menu(menu)
	{}
};

