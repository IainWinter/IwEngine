#pragma once

#include "iw/engine/System.h"
#include "Components/Flocker.h"
#include "ECS.h"

struct FlockingSystem
	: iw::SystemBase
{
	FlockingSystem() : iw::SystemBase("Flocking") {}

	void FixedUpdate() override;
};
