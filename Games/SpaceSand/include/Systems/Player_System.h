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
		auto [w, h] = sand->GetSandTexSize2();

		player = sand->MakeTile<iw::Circle, Player>("textures/SpaceGame/player.png", true);

		Player*        p = player.Set<Player>();
		iw::Circle*    c = player.Find<iw::Circle>();
		iw::Rigidbody* r = player.Find<iw::Rigidbody>();

		p->timer.SetTime("fire1", 0.15f);
		p->timer.SetTime("fire2", 0.015f);

		c->Radius = 4;

		r->Transform.Position = glm::vec3(w, h, 0);
		r->SetMass(10);
	}

	void FixedUpdate() override;
	void Update() override;
};
