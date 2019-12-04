#pragma once
#include "Collider.h"
#include "iw/math/vector3.h"

namespace IW {
inline namespace Physics {
	struct SphereCollider
	: Collider<iw::vector3>
	{
		float Radius;

		SphereCollider(
			iw::vector3 center,
			float radius);

		iw::vector3 Center() const;
		
		bool IsColliding(const SphereCollider& other) const override;
	};
}
}
