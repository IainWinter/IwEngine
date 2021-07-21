#pragma once

struct Player
{
	
};

#include "plugins/iw/Sand/Engine/SandLayer.h"

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
	{}

	int  Initialize() override;
	void FixedUpdate() override;
	void Update() override;
};
