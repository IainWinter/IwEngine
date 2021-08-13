#pragma once

#include <functional>

struct Projectile {
	std::function<void()> FixedUpdate; // called in physics tick
	std::function<std::tuple<bool, int, int>()> Update;      // called in render tick, return true if collision

	std::function<void(int, int)> OnHit; // called on collision
};
