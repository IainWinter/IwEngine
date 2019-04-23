#pragma once

#include "iw/physics/Collision/Collider.h"

namespace IwPhysics {
	struct CollisionData {
		const Collider& First;
		const Collider& Second;
		const iwm::vector3 Intersect;
	};
}
