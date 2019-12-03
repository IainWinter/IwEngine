#include "iw/physics/collision/SphereCollider.h"
#include "iw/physics/collision/Collider.h"
#include "iw/math/vector3.h"

namespace IW {
	SphereCollider::SphereCollider(
		iw::vector3 center,
		float radius)
		: Collider<iw::vector3>(1, new iw::vector3[1])
	{
		Points[0] = center;
		Radius = radius;
	}

	iw::vector3 SphereCollider::Center() const {
		return Points[0];
	}

	bool SphereCollider::IsColliding(
		const SphereCollider &other) const
	{
		return Radius + other.Radius < (Center() + other.Center()).length;
	}
}
