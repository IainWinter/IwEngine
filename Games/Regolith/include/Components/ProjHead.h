#pragma once

#include "ECS.h"
#include <vector>

// forward declare Projectile for Events.h

struct Projectile;

// component wrapper for oo

enum ProjectileType {
	BULLET, // these are the same actually
	LASER,
	BEAM,
	WATTZ,
	BOLTZ
};

struct ShotInfo {
	float x, y, dx, dy, life;
	ProjectileType projectile;
	//iw::Entity origin;
	entity origin;

	float Speed() const {
		return sqrt(dx * dx + dy * dy);
	}

	std::vector<ShotInfo> others;
};

inline ShotInfo GetShot_Circular(
	float x, float y,
	float tx, float ty,
	float speed, float margin, float thickness = 0)
{
	float dx = tx - x,
		dy = ty - y;

	float length = sqrt(dx * dx + dy * dy);

	float nx = dx / length,
		ny = dy / length;

	dx = nx * speed;
	dy = ny * speed;

	float r = iw::randfs();

	x += nx * margin - ny * thickness * r;
	y += ny * margin + nx * thickness * r;

	return ShotInfo{ x, y, dx, dy, 0.f };
}
