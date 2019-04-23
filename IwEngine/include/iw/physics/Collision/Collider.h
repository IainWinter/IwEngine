#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/AABB.h"
#include "iw/math/matrix4.h"

namespace IwPhysics {
	struct Collider {
		AABB Bounds;

		virtual iwm::vector3 Support(
			const iwm::matrix4& transformation) const = 0;
	};
}
