#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/Health.h"
#include "iw/physics/Collision/SphereCollider.h"

#include "Events.h"

struct HealthSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	HealthSystem(
		iw::SandLayer* sand,
		iw::Entity player
	)
		: iw::SystemBase("Health")
		, sand(sand)
		, m_player(player)
	{}

	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	iw::Entity MakeHealth(float x, float y, float ndx, float dny);
};
