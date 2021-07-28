#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/Projectile.h"

struct ProjectileSystem
	: iw::SystemBase
{
	iw::SandLayer* sand;

	std::vector<std::tuple<int, int, float>> m_cells; // x, y, life

	ProjectileSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Projectile")
		, sand(sand)
	{}

	int Initialize() override;
	void Update() override;
	void FixedUpdate() override;

	// Make functions, not sure where these should go I
	// guess its up to the caller to deside, could add an action evevnt

	iw::Entity MakeBulletC(float x, float y, float dx, float dy, int maxDepth = 5, int depth = 0); // in cell space
	iw::Entity MakeBullet (float x, float y, float dx, float dy, int maxDepth = 5, int depth = 0);


	void MakeExplosion(int x, int y, int r);
};
