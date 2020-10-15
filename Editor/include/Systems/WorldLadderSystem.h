#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"
#include "Components/GameSaveState.h"

class WorldLadderSystem
	: public iw::SystemBase
{
private:
	iw::Entity currentLevel; // get rid of thjis
	GameSaveState* saveState;

	iw::Prefab ladderPrefab;

public:
	WorldLadderSystem(
		GameSaveState* saveState);

	int Initialize()   override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnLadder(
		iw::vector3 position,
		iw::quaternion rotation,
		bool* down_saveState,
		bool above,
		std::string level);

};
