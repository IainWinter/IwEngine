#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Events.h"
#include <utility>
#include <functional>

// helpers

inline std::tuple<float, float> randvel(
	iw::Entity entity,
	float amount,
	bool setForMe = false) 
{
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
	float dx = r->Velocity.x;
	float dy = r->Velocity.y;

	float speed = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx);
	angle += iw::randfs() * amount;

	dx = cos(angle) * speed;
	dy = sin(angle) * speed;

	if (setForMe) {
		entity.Find<iw::Rigidbody>()->Velocity = glm::vec3(dx, dy, 0);
	}

	return std::tuple(dx, dy);
}

// end helpers

struct HitInfo {
	bool hit = false;
	float x, y;

	iw::SandLayer* sand;
	iw::ref<iw::eventbus> bus;
	iw::Entity entity;
};

struct Projectile
{
	ShotInfo Shot;

	iw::Cell Cell;
	float Life = 0.f;

	Projectile() {
		Cell.Type = iw::CellType::PROJECTILE;
	}

	virtual bool OnUpdate(
		iw::Entity entity)
	{
		return false;
	}

	virtual void DrawCell(
		iw::SandLayer* sand,
		iw::SandChunk* chunk,
		int x, int y,
		float dx, float dy)
	{
		chunk->SetCell(x, y, Cell);
	}

	virtual void OnHit(
		HitInfo hit) = 0;
};

struct Split_Projectile : Projectile
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

	Split_Projectile(
		int split
	)
		: Projectile()
		, Split(split)
	{}

	void OnHit(HitInfo hit) override
	{
		int hx = floor(hit.x);
		int hy = floor(hit.y);

		if (    Split < MaxSplit
			&& hit.sand->m_world->InBounds(hx, hy))
		{
			glm::vec3& pos = hit.entity.Find<iw::Transform>()->Position;
			glm::vec3& vel = hit.entity.Find<iw::Rigidbody>()->Velocity;

			for (int i = 0; i < ShrapCount + 1; i++)
			{
				if (i == 0 || iw::randf() > ShrapOdds)
				{
					continue;
				}

				float speed = glm::length(vel);
				auto [dx2, dy2] = randvel(hit.entity, i == 0 ? SplitTurnArc : ShrapTurnArc);

				ShotInfo split = Shot;
				split.x = hit.x;
				split.y = hit.y;
				split.dx = dx2;
				split.dy = dy2;

				hit.bus->push<SpawnProjectile_Event>(split, Split + 1);
			}
		}

		hit.entity.Queue(iw::func_Destroy);
	}
};

inline Split_Projectile* MakeBullet_Projectile(int split)
{
	Split_Projectile* proj = new Split_Projectile(split);
	proj->Cell.life = .02;
	proj->Cell.Color = iw::Color::From255(255, 230, 66);
	proj->SplitTurnArc = iw::Pi / 4;
	proj->MaxSplit = 15;

	return proj;
}

inline Split_Projectile* MakeLaser_Projectile(int split)
{
	Split_Projectile* proj = new Split_Projectile(split);
	proj->Cell.life = .2f;
	proj->Cell.Color = iw::Color::From255(255, 23, 6);
	proj->MaxSplit = 25;
	proj->ShrapCount = 1;
	proj->ShrapOdds = .1f;
	proj->ShrapTurnArc = iw::Pi / 6;

	return proj;
}
