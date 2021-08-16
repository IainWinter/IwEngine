#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/Projectile.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "Events.h"

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

	bool On(iw::ActionEvent& e) override;

	// Make functions, not sure where these should go I
	// guess its up to the caller to deside, should add an action evevnt
private:
	struct ProjectileInfo {
		iw::Entity entity;
		Projectile* projectile;
		std::function<std::tuple<float, float, int, float, float>()>             getpos;
		std::function<void                                       (float, float)> setpos;
		std::function<void                                       (float, float)> setvel;
		std::function<std::tuple<float, float>                   (float, bool)>  randvel;
	};

	ProjectileInfo MakeProjectile(
		float x,  float  y,
		float dx, float dy,
		std::function<void(iw::SandChunk*, int, int)> placeCell);

	iw::Entity MakeBullet(float x, float y, float dx, float dy, int maxDepth = 5, int depth = 0);
	iw::Entity MakeLaser (float x, float y, float dx, float dy, int maxDepth = 5, int depth = 0);

	void MakeExplosion(int x, int y, int r);
};
