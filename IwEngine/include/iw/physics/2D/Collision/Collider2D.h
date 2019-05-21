#pragma once

#include "iw/physics/IwPhysics.h"
#include "AABB2D.h"
#include "iw/math/matrix4.h"

namespace IwPhysics {
	struct Collider2D {
		AABB2D Bounds;

		Collider2D() = default;

		Collider2D(
			const AABB2D& bounds);

		virtual ~Collider2D() = default;

		virtual iwm::vector2 FurthestPoint(
			const iwm::vector2& direction) const = 0;
	};
}
