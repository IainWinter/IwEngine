#pragma once

#include "iw/engine/System.h"
#include "Components/EnemyShip.h"

struct EnemySystem : iw::SystemBase
{
	EnemySystem()
		: iw::SystemBase("Enemy")
	{}

	//int Initialize() override;
	void FixedUpdate() override;
};
