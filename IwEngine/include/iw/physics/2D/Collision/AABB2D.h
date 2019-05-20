#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/math/vector2.h"

namespace IwPhysics {
	struct IWPHYSICS_API AABB2D {
		iwm::vector2 Min;
		iwm::vector2 Max;

		AABB2D(
			iwm::vector2 center,
			float scale);

		AABB2D(
			iwm::vector2 min,
			iwm::vector2 max);

		iwm::vector2 Mid() const;

		bool Intersects(
			const AABB2D& other) const;

		bool Fits(
			const AABB2D& other) const;
	};
}
