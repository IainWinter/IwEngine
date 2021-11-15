#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/ProjHead.h"

#include "Events.h"
#include "Helpers.h"

#include <utility>
#include <functional>

struct HitInfo {
	bool HasContact = false;
	float x, y;

	iw::Entity Entity;
};

struct Projectile
{
	const ShotInfo Shot;
	HitInfo Hit;

	float Life = 0.f;

	Projectile() {}

	Projectile(
		const ShotInfo& shot
	)
		: Shot(shot)
		, Life(shot.life)
	{}
};

struct Linear_Projectile
{
	iw::Cell Cell;
};

struct Split_Projectile
{
	// Split is how any shot continues through cells
	int Split = 0;
	int MaxSplit = 0;
	float SplitTurnArc = 0.f;

	// shrap is other shots spawned with the split
	int ShrapCount = 0;
	int ShrapOdds = .1f;
	//float ShrapSpeedRatio = 1.f;
	float ShrapTurnArc = 0.f;

	Split_Projectile() {}

	Split_Projectile(
		int depth
	)
		: Split(depth)
	{}
};

struct Tile_Projectile
{
	iw::Tile Tile;

	Tile_Projectile() {}
};

struct LightBall_Projectile
{
	std::vector<std::pair<float, float>> Points;
	iw::Timer Timer;

	LightBall_Projectile() {}
};

// tag
struct LightBolt_Projectile
{
};
