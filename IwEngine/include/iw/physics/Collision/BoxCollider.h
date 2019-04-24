#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/Collider.h"

namespace IwPhysics {
	struct IWPHYSICS_API BoxCollider
		: Collider
	{
		BoxCollider(
			const AABB& bounds);

		iwm::vector3 Support(
			const iwm::matrix4& transformation) const override;
	};
}
