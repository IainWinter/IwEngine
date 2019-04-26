#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/AABB.h"
#include "iw/math/matrix4.h"

namespace IwPhysics {
	struct Collider {
		AABB Bounds;

		Collider(
			const AABB& bounds);

		inline AABB Translated(
			iwm::matrix4 translation) const
		{
			return AABB(
				Bounds.Min * translation,
				Bounds.Max * translation);
		}

		virtual iwm::vector3 FurthestPoint(
			const iwm::matrix4& transformation,
			const iwmath::vector3& direction) const = 0;
	};
}
