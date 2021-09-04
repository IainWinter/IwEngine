#pragma once

#include "iw/engine/System.h"
#include "Components/EnemyShip.h"
#include "Events.h"

struct ScoreSystem : iw::SystemBase
{
	int Score;

	ScoreSystem()
		: iw::SystemBase("Score")
	{}

	//void Update() override;

	bool On(iw::ActionEvent& e) override;
};