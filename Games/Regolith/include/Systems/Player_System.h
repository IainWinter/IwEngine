#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "Components/Player.h"
#include "Components/KeepInWorld.h"
#include "Components/Item.h"
#include "Components/CorePixels.h"

#include "Events.h"
#include "iw/engine/Assets.h"
#include "Helpers.h"

struct PlayerSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	iw::HandlerFunc* m_handle;

	PlayerSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Player")
		, sand(sand)
	{}

	int Initialize() override;
	void Destroy() override;
	void FixedUpdate() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
