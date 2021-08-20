#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Components/Timer.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/EnemyShip.h"
#include "Components/Player.h"
#include "Components/Flocker.h"

#include "Events.h"
#include "Levels.h"

struct WorldSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	iw::Timer m_timer;

	std::vector<iw::EventSequence> m_levels;

	WorldSystem(
		iw::SandLayer* sand,
		iw::Entity player
	)
		: iw::SystemBase("World")
		, sand(sand)
		, m_player(player)
	{}

	int Initialize() override;
	void Update() override;
	void FixedUpdate() override;
};
