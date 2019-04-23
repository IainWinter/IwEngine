#pragma once

#include "iw/physics/Collision/Collider.h"

namespace IwPhysics {
	struct BoxCollider
		: Collider
	{
		iwm::vector3 Support(
			const iwm::matrix4& transformation) const override;
	};
}
