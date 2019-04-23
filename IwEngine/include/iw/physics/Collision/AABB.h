#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/math/vector3.h"

namespace IwPhysics {
	struct IWPHYSICS_API AABB {
		iwm::vector3 Min;
		iwm::vector3 Max;

		AABB(iwm::vector3 center, float scale);
		AABB(iwm::vector3 min, iwm::vector3 max);

		iwm::vector3 Mid() const;

		bool Intersects(
			const AABB& other) const;

		bool Fits(
			const AABB& other) const;

	};
}
