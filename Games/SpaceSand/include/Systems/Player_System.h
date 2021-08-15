#pragma once

#include "Components/Player.h"
#include "Systems/Projectile_System.h"
#include "Helpers.h"
#include "Events.h"

struct PlayerSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity player;

	float cam_x, cam_y;

	PlayerSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Player")
		, sand(sand)
		, cam_x(0)
		, cam_y(0)
	{
		player = sand->MakeTile("textures/SpaceGame/player.png", true);
	}

	int  Initialize() override;
	void FixedUpdate() override;
	void Update() override;
};
