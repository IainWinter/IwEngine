#pragma once

#include "iw/engine/Events/ActionEvents.h"

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"

enum class Actions
	: int
{
	DEV_CONSOLE,
	JUMP, RIGHT, FORWARD, USE,
	RESET_LEVEL, START_LEVEL, UNLOCK_LEVEL_DOOR, LOAD_NEXT_LEVEL, GOTO_NEXT_LEVEL, AT_NEXT_LEVEL,
	SPAWN_ENEMY_DEATH
};

struct DevConsoleEvent
	: iw::ToggleEvent
{
	DevConsoleEvent(
		bool active)
		: ToggleEvent(iw::val(Actions::DEV_CONSOLE), active)
	{}
};

struct JumpEvent
	: iw::ToggleEvent
{
	JumpEvent(
		bool active)
		: ToggleEvent(iw::val(Actions::JUMP), active)
	{}
};

struct RightEvent
	: iw::ToggleEvent
{
	RightEvent(
		bool active)
		: iw::ToggleEvent(iw::val(Actions::RIGHT), active)
	{}
};

struct ForwardEvent
	: iw::ToggleEvent
{
	ForwardEvent(
		bool active)
		: iw::ToggleEvent(iw::val(Actions::FORWARD), active)
	{}
};

struct UseEvent
	: iw::SingleEvent
{
	UseEvent()
		: iw::SingleEvent(iw::val(Actions::USE))
	{}
};

struct ResetLevelEvent
	: iw::SingleEvent
{
	ResetLevelEvent()
		: iw::SingleEvent(iw::val(Actions::RESET_LEVEL))
	{}
};

struct StartLevelEvent
	: iw::SingleEvent
{
	bool CameraFollow;
	iw::vector2 PlayerPosition;

	StartLevelEvent(
		bool cameraFollow,
		iw::vector2 playerPosition)
		: iw::SingleEvent(iw::val(Actions::START_LEVEL))
		, CameraFollow(cameraFollow)
		, PlayerPosition(playerPosition)
	{}
};


struct UnlockLevelDoorEvent
	: iw::SingleEvent
{
	UnlockLevelDoorEvent()
		: iw::SingleEvent(iw::val(Actions::UNLOCK_LEVEL_DOOR))
	{}
};

struct LoadNextLevelEvent
	: iw::SingleEvent
{
	LoadNextLevelEvent()
		: iw::SingleEvent(iw::val(Actions::LOAD_NEXT_LEVEL))
	{}
};

struct GoToNextLevelEvent
	: iw::SingleEvent
{
	bool CameraFollow;
	iw::vector2 PlayerPosition;
	iw::vector2 CenterPosition;

	GoToNextLevelEvent(
		bool cameraFollow,
		iw::vector2 playerPosition,
		iw::vector2 centerPosition)
		: iw::SingleEvent(iw::val(Actions::GOTO_NEXT_LEVEL))
		, CameraFollow(cameraFollow)
		, PlayerPosition(playerPosition)
		, CenterPosition(centerPosition)
	{}
};

struct AtNextLevelEvent
	: iw::SingleEvent
{
	AtNextLevelEvent()
		: iw::SingleEvent(iw::val(Actions::AT_NEXT_LEVEL))
	{}
};

struct SpawnEnemyDeath
	: iw::SingleEvent
{
	iw::vector3 Position;

	SpawnEnemyDeath(
		iw::vector3 position)
		: iw::SingleEvent(iw::val(Actions::SPAWN_ENEMY_DEATH))
		, Position(position)
	{}
};
