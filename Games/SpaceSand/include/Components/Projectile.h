#pragma once

#include <functional>

struct Projectile {
	std::function<void()> Update;
	std::function<void()> OnHit;
	std::function<bool()> TestForHit;
};
