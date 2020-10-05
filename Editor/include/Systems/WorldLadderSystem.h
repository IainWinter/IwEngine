#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"
#include "Components/GameSaveState.h"

class WorldLadderSystem
	: public iw::SystemBase
{
private:
	iw::Entity& currentLevel;
	GameSaveState* saveState;

	iw::Prefab ladderPrefab;

public:
	WorldLadderSystem(
		iw::Entity& currentLevel,
		GameSaveState* saveState);

	int Initialize()   override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	void SpawnLadder(
		iw::vector3 position,
		iw::quaternion rotation,
		bool down,
		bool above,
		std::string level);

};
