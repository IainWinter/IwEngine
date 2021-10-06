#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Events.h"
#include "iw/engine/Assets.h"

struct PlayerLaserTankSystem : iw::SystemBase
{
	iw::Entity m_player;
	iw::SandLayer* m_tankSand;

	glm::vec3 m_fluidVelocity;
	int m_fluidToCreate;
	int m_fluidToRemove;
	int m_fluidCount;

	float m_fluidTime;
	float m_perCellTime = .05f;

	PlayerLaserTankSystem(
		iw::SandLayer* tankSand
	)
		: iw::SystemBase("Player laser tank")
		, m_tankSand(tankSand)
	{}

	void OnPush() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
