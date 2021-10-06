#pragma once

#include "iw/engine/System.h"

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Events.h"
#include "iw/engine/Assets.h"

struct ExplosionSystem : iw::SystemBase
{
	iw::SandLayer* sand;

	ExplosionSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Explosion")
		, sand(sand)
	{}

	bool On(iw::ActionEvent& e) override;

private:
	void SpawnExplosion(SpawnExplosion_Config& config);
};
