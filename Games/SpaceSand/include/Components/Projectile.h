#pragma once

#include <functional>

struct Projectile {
	std::function<void()> FixedUpdate; // called in physics tick
	std::function<std::tuple<bool, float, float>()> Update; // called in render tick, return true if collision

	std::function<void(float, float)> OnHit; // called on collision
};
