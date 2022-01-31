#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "Components/Player.h"
#include "Components/KeepInWorld.h"
#include "Components/Item.h"
#include "Components/CorePixels.h"

#include "Events.h"
#include "Assets.h"
#include "Helpers.h"
#include "Moves.h"

#include "iw/engine/Layers/ImGuiLayer.h"

struct PlayerSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;
	iw::HandlerFunc* m_handle;
	std::vector<float> speeds;
	iw::Timer timer;

	PlayerSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase ("Player")
		, sand           (sand)
		, m_handle       (nullptr)
	{
		timer.SetTime("damage sound", .1f, .25f);
	}

	int Initialize() override;
	void Destroy() override;
	void FixedUpdate() override;
	void Update() override;

	void ImGui() override;

	bool On(iw::ActionEvent& e) override;

	// player moves0

	void FireWeaponMain();
	void FireWeaponAlt();

	void UseA();
	void UseB();
	void UseX();
	void UseY();
};
