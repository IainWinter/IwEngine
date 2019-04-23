#pragma once

#include "iw/math/vector3.h"

namespace IwPhysics {
	struct AABB {
		iwmath::vector3 Min;
		iwmath::vector3 Max;

		iwmath::vector3 Mid() const;

		bool Intersects(
			const AABB& other) const;

		bool Fits(
			const AABB& other) const;
	};
}
