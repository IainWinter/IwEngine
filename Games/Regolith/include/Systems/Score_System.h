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

	bool On(iw::ActionEvent& e) override;
};
