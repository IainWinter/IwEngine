#pragma once

#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"
#include "plugins/iw/Sand/Defs.h"
#include "Components/Player.h"
#include "Components/Item.h"
#include "Components/Projectile.h"
#include "Components/EnemyShip.h"
#include "iw/util/reflection/ReflectDef.h"
#include "State.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
	SPAWN_ITEM,
	SPAWN_ASTEROID,
	SPAWN_EXPLOSION,
	CHANGE_LASER_FLUID,
	CHANGE_PLAYER_WEAPON,
	CHANGE_PLAYER_SCORE,
	HEAL_PLAYER,
	PROJ_HIT_TILE,
	REMOVE_CELL_FROM_TILE,
	CREATED_PLAYER,
	CREATED_CORE_TILE,
	CORE_EXPLODED,
	STATE_CHANGE,
	OPEN_MENU,
};

struct REFLECT SpawnExplosion_Config
{
	float SpawnLocationX  =   0.f;
	float SpawnLocationY  =   0.f;
	float ExplosionRadius =  10.f;
	float ExplosionPower  = 200.f;
};

struct REFLECT SpawnExplosion_Event : iw::SingleEvent
{
	SpawnExplosion_Config Config;

	SpawnExplosion_Event(
		SpawnExplosion_Config& config
	)
		: iw::SingleEvent(SPAWN_EXPLOSION)
		, Config(config)
	{}
};

struct REFLECT SpawnEnemy_Config
{
	iw::Entity TargetEntity;
	float SpawnLocationX  = 0.f;
	float SpawnLocationY  = 0.f;
	float TargetLocationX = 0.f;
	float TargetLocationY = 0.f;

	EnemyType EnemyType = EnemyType::FIGHTER;
};

struct REFLECT SpawnEnemy_Event : iw::SingleEvent
{
	SpawnEnemy_Config Config;

	SpawnEnemy_Event(
		SpawnEnemy_Config config
	)
		: iw::SingleEvent(SPAWN_ENEMY)
		, Config(config)
	{}
};

struct REFLECT SpawnAsteroid_Config
{
	float SpawnLocationX = 0.f;
	float SpawnLocationY = 0.f;
	float VelocityX      = 0.f;
	float VelocityY      = 0.f;
	float AngularVel     = 0.f;
	int Size = 1;
};

struct REFLECT SpawnAsteroid_Event : iw::SingleEvent
{
	SpawnAsteroid_Config Config;

	SpawnAsteroid_Event(
		const SpawnAsteroid_Config& config
	)
		: iw::SingleEvent(SPAWN_ASTEROID)
		, Config(config)
	{}
};

struct REFLECT SpawnProjectile_Event : iw::SingleEvent
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

struct REFLECT SpawnItem_Config
{
	float X             =  0.f; // should be named SpawnLocationX
	float Y             =  0.f;
	float ActivateDelay =  0.f;
	float AngularSpeed  =  0.f;
	float Speed         = 75.f;

	int Amount          = 1;
	bool DieWithTime    = true;

	ItemType Item;
	std::function<void()> OnPickup;
};

struct REFLECT SpawnItem_Event : iw::SingleEvent
{
	SpawnItem_Config Config;

	SpawnItem_Event(
		const SpawnItem_Config& config
	)
		: iw::SingleEvent(SPAWN_ITEM)
		, Config(config)
	{}
};

struct REFLECT ChangeLaserFluid_Event : iw::SingleEvent
{
	int Amount;

	ChangeLaserFluid_Event(
		int amount
	)
		: iw::SingleEvent(CHANGE_LASER_FLUID)
		, Amount(amount)
	{}
};

struct REFLECT ChangePlayerWeapon_Event : iw::SingleEvent
{
	WeaponType Type;
	Weapon* CurrentWeapon;

	ChangePlayerWeapon_Event(
		WeaponType weapon
	)
		: iw::SingleEvent(CHANGE_PLAYER_WEAPON)
		, Type(weapon)
		, CurrentWeapon(nullptr)
	{}
};

struct REFLECT ChangePlayerScore_Event : iw::SingleEvent
{
	int Score;

	ChangePlayerScore_Event(
		int score
	)
		: iw::SingleEvent(CHANGE_PLAYER_SCORE)
		, Score(score)
	{}
};

struct REFLECT HealPlayer_Event : iw::SingleEvent
{
	bool IsCore;

	HealPlayer_Event(
		int isCore = false
	)
		: iw::SingleEvent(HEAL_PLAYER)
		, IsCore(isCore)
	{}
};

struct REFLECT ProjHitTile_Config
{
	int X = 0;
	int Y = 0;
	iw::TileInfo Info;
	iw::Entity Hit;
	iw::Entity Projectile;
};

struct REFLECT ProjHitTile_Event : iw::SingleEvent
{
	ProjHitTile_Config Config;
	
	ProjHitTile_Event(
		const ProjHitTile_Config& config
	)
		: iw::SingleEvent(PROJ_HIT_TILE)
		, Config(config)
	{}
};

struct REFLECT RemoveCellFromTile_Event : iw::SingleEvent
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

struct REFLECT CreatedPlayer_Event : iw::SingleEvent
{
	iw::Entity PlayerEntity;

	CreatedPlayer_Event(
		iw::Entity playerEntity
	)
		: iw::SingleEvent(CREATED_PLAYER)
		, PlayerEntity(playerEntity)
	{}
};

struct REFLECT CreatedCoreTile_Event : iw::SingleEvent
{
	iw::Entity TileEntity;

	CreatedCoreTile_Event(
		iw::Entity tileEntity
	)
		: iw::SingleEvent(CREATED_CORE_TILE)
		, TileEntity(tileEntity)
	{}
};


struct REFLECT CoreExploded_Event : iw::SingleEvent
{
	iw::Entity Entity;

	CoreExploded_Event(
		iw::Entity entity
	)
		: iw::SingleEvent(CORE_EXPLODED)
		, Entity(entity)
	{}
};

struct REFLECT StateChange_Event : iw::SingleEvent
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

struct REFLECT OpenMenu_Event : iw::SingleEvent
{
	MenuName Menu;

	OpenMenu_Event(
		MenuName menu
	)
		: iw::SingleEvent(OPEN_MENU)
		, Menu(menu)
	{}
};

