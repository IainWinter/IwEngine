#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/Projectile.h"

struct ProjectileSystem
	: iw::SystemBase
{
	iw::SandLayer* sand;

	ProjectileSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Projectile")
		, sand(sand)
	{}

	int Initialize() override;
	void FixedUpdate() override;

	// Make functions, not sure where these should go I
	// guess its up to the caller to deside, could add an action evevnt

	iw::Entity MakeBulletC(float x, float y, float dx, float dy); // in cell space
	iw::Entity MakeBullet(float x, float y, float dx, float dy);
};
