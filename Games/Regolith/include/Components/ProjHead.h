#pragma once

#include <vector>

// forward declare Projectile for Events.h

struct Projectile;

// component wrapper for oo

struct ProjHead {

	Projectile* Proj;

	ProjHead() : Proj(nullptr) {}

	// takes ownership
	ProjHead(
		Projectile* proj
	)
		: Proj(proj)
	{}

	~ProjHead()
	{
		delete Proj;
	}
};

enum ProjectileType {
	BULLET, // these are the same actually
	LASER,
	BEAM
};

struct ShotInfo {
	float x, y, dx, dy, life;
	ProjectileType projectile;

	float Speed() {
		return sqrt(dx * dx + dy * dy);
	}

	std::vector<ShotInfo> others;
};

inline ShotInfo GetShot_Circular(
	float x,  float y,
	float tx, float ty,
	float speed, float margin, float thickness = 0)
{
	float dx = tx - x,
	      dy = ty - y;

	float length = sqrt(dx*dx + dy*dy);

	float nx = dx / length,
           ny = dy / length;

	dx = nx * speed;
	dy = ny * speed;

	float r = iw::randfs();

	x += nx * margin - ny * thickness * r;
	y += ny * margin + nx * thickness * r;

	return ShotInfo { x, y, dx, dy, 0.f };
}
