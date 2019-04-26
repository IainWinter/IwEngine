#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/Collider.h"

namespace IwPhysics {
	struct IWPHYSICS_API BoxCollider
		: Collider
	{
		BoxCollider(
			const AABB& bounds);

		iwm::vector3 FurthestPoint(
			const iwm::matrix4& transformation,
			const iwmath::vector3& direction) const override;
	};
}
