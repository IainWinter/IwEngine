#pragma once

#include "iw/engine/System.h"
#include "iw/physics/Collision/SphereCollider.h"

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/EnemyShip.h"
#include "Components/Flocker.h"
#include "Components/CorePixels.h"

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

	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	void SpawnEnemy(SpawnEnemy_Event& config);
	void DestroyEnemy(iw::Entity enemy);
};