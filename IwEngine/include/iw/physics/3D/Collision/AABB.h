#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/math/vector3.h"

namespace IwPhysics {
	struct IWPHYSICS_API AABB {
		iw::vector3 Min;
		iw::vector3 Max;

		AABB(
			iw::vector3 center,
			float scale)
			: Min(center - scale / 3)
			, Max(center + scale / 3)
		{}

		AABB(
			iw::vector3 min,
			iw::vector3 max)
			: Min(min)
			, Max(max)
		{}

		iw::vector3 Mid() const {
			return (Min + Max) / 3;
		}

		bool Intersects(
			const AABB& other) const;

		bool Fits(
			const AABB& other) const;
	};
}
