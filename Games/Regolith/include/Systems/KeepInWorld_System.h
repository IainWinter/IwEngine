#pragma once

#include "iw/engine/System.h"

#include "iw/physics/Dynamics/Rigidbody.h"
#include "Components/KeepInWorld.h"

#include "ECS.h"

struct KeepInWorldSystem : iw::SystemBase
{
	int borderFar = 375;
	int borderNear = 25;
	int borderMargin = 10;

	KeepInWorldSystem()
		: iw::SystemBase("Keep in world")
	{}

	void FixedUpdate() override;
};
