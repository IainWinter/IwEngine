#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/engine/Components/Timer.h"
#include "Components/EnemyShip.h"
#include "Components/Player.h"
#include "Components/Flocker.h"
#include "Components/Asteroid.h"

#include "Events.h"
#include "Levels.h"
#include "Assets.h"

struct WorldSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	iw::Timer m_timer;

	std::vector<iw::EventSequence> m_levels;

	WorldSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("World")
		, sand(sand)
	{}

	void OnPush() override;
	void Update() override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	iw::Entity MakeAsteroid(SpawnAsteroid_Config& config);

	Spawn::func_Spawn SpawnEnemy = [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
	};


	void SetupLevels();
};
