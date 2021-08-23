#pragma once

#include "plugins/iw/Sand/SandChunk.h"
#include <utility>
#include <functional>

enum ProjectileType {
	BULLET,
	LASER
};

struct Projectile {
	std::function<void()> FixedUpdate; // called in physics tick
	std::function<std::tuple<bool, float, float>()> Update; // called in render tick, return true if collision

	std::function<void(float, float)> OnHit; // called on collision

	std::function<void(iw::SandChunk*, int, int, float, float)> PlaceCell; // each cell in line calls this
};
