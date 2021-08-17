#pragma once

#include <functional>

struct Item {
	float MoveTime = 2.f;
	float MoveTimer = 0.f;

	bool PickingUp = false;
	std::function<void()> OnPickUp;
};
