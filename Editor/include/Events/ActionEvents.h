#pragma once

#include "iw/engine/Events/ActionEvents.h"

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/common/Components/Transform.h"

enum class Actions
	: int
{
	GAME_STATE, DEV_CONSOLE,
	JUMP, RIGHT, FORWARD, USE,
	RESET_LEVEL, START_LEVEL, UNLOCK_LEVEL_DOOR, LOAD_NEXT_LEVEL, GOTO_NEXT_LEVEL, AT_NEXT_LEVEL,
	SPAWN_ENEMY_DEATH, SPAWN_ITEM, SPAWN_NOTE, SPAWN_CONSUMABLE
};

struct DevConsoleEvent
	: iw::ToggleEvent
{
	DevConsoleEvent(
		bool active)
		: ToggleEvent(iw::val(Actions::DEV_CONSOLE), active)
	{}
};

enum GameState {
	PAUSED,
	RUNNING
};

struct GameStateEvent
	: iw::SingleEvent
{
	GameState State;

	GameStateEvent(
		GameState state)
		: iw::SingleEvent(iw::val(Actions::GAME_STATE))
		, State(state)
	{}
};

struct JumpEvent
	: iw::ToggleEvent
{
	JumpEvent(
		bool active)
		: iw::ToggleEvent(iw::val(Actions::JUMP), active)
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
	std::string LevelName;
	iw::vector2 Position;
	bool GoBack;

	LoadNextLevelEvent()
		: iw::SingleEvent(iw::val(Actions::LOAD_NEXT_LEVEL))
	{}

	LoadNextLevelEvent(
		std::string levelName,
		iw::vector2 position,
		bool goback)
		: iw::SingleEvent(iw::val(Actions::LOAD_NEXT_LEVEL))
		, LevelName(levelName)
		, Position(position)
		, GoBack(goback)
	{}
};

struct GoToNextLevelEvent
	: iw::SingleEvent
{
	std::string LevelName;
	bool CameraFollow;
	iw::vector2 PlayerPosition;
	iw::vector2 CenterPosition;

	GoToNextLevelEvent(
		std::string levelName,
		bool cameraFollow,
		iw::vector2 playerPosition,
		iw::vector2 centerPosition)
		: iw::SingleEvent(iw::val(Actions::GOTO_NEXT_LEVEL))
		, LevelName(levelName)
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
	iw::Transform* Level;

	SpawnEnemyDeath(
		iw::vector3 position,
		iw::Transform* level)
		: iw::SingleEvent(iw::val(Actions::SPAWN_ENEMY_DEATH))
		, Position(position)
		, Level(level)
	{}
};

struct SpawnItemEvent
	: iw::SingleEvent
{
	iw::vector3 Position;
	iw::Transform* Level;

	SpawnItemEvent(
		iw::vector3 position,
		iw::Transform* level)
		: iw::SingleEvent(iw::val(Actions::SPAWN_ITEM))
		, Position(position)
		, Level(level)
	{}
};

struct SpawnNoteEvent
	: iw::SingleEvent
{
	int Index;

	SpawnNoteEvent(
		int index)
		: iw::SingleEvent(iw::val(Actions::SPAWN_NOTE))
		, Index(index)
	{}
};

struct SpawnConsumableEvent
	: iw::SingleEvent
{
	int Index;

	SpawnConsumableEvent(
		int index)
		: iw::SingleEvent(iw::val(Actions::SPAWN_CONSUMABLE))
		, Index(index)
	{}
};
