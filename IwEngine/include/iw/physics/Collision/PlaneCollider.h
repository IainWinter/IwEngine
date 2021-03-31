#pragma once

#include "Collider.h"
#include "iw/physics/Plane.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT PlaneCollider
		: Collider<V>
	{
		REFLECT impl::Plane<V> Plane;

		PlaneCollider()
			: Collider<V>(ColliderType::PLANE)
			, Plane(0.0f, 0.0f)
		{}

		PlaneCollider(
			V p,
			float d)
			: Collider<V>(ColliderType::PLANE)
			, Plane(p, d)
		{}

		PlaneCollider(
			impl::Plane<V> plane)
			: Collider<V>(ColliderType::PLANE)
			, Plane(plane)
		{}

		const AABB<V>& Bounds() override {
			//if (m_outdated) {
			//	m_bounds = AABB<V>(XYZ, D);
			//	m_outdated = false;
			//}

			return m_bounds;
		}

		Transform Trans() const override {
			Transform transform;
			transform.Position = Plane.P.normalized() * Plane.D;
			transform.Scale    = 10.0f; // should be width, height when made non infinite

			transform.Rotation =  quaternion::from_look_at(-Plane.P);
			transform.Rotation *= quaternion::from_axis_angle(transform.Right(), -Pi / 2);

			return transform;
		}
	};
}
}

	using namespace Physics;
}
