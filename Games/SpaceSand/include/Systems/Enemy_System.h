#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/EnemyShip.h"
#include "Components/Flocker.h"
#include "Helpers.h"
#include "Events.h"

struct EnemySystem : iw::SystemBase
{
	iw::SandLayer* sand;

	EnemySystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Enemy")
		, sand(sand)
	{}

	//int Initialize() override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

	void SpawnEnemy(SpawnEnemy_Event& config);
};
