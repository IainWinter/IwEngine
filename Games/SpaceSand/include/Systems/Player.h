#pragma once

#include "Components/Player.h"
#include "Systems/Projectile.h"

struct PlayerSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	ProjectileSystem* guns;
	iw::Entity player;

	float cam_x, cam_y;

	PlayerSystem(
		iw::SandLayer* sand,
		ProjectileSystem* guns
	)
		: iw::SystemBase("Player")
		, sand(sand)
		, guns(guns)
		, cam_x(0)
		, cam_y(0)
	{}

	int  Initialize() override;
	void FixedUpdate() override;
	void Update() override;
};
