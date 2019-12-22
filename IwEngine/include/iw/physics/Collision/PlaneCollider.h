#pragma once

#include "Collider.h"
#include "iw/physics/Plane.h"

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct PlaneCollider
		: Collider<V>
	{
		impl::Plane<V> Plane;

		PlaneCollider(
			V p,
			float d)
			: Collider<V>(ColliderShape::PLANE)
			, Plane(p, d)
		{}

		PlaneCollider(
			impl::Plane<V> plane)
			: Collider<V>(ColliderShape::PLANE)
			, Plane(plane)
		{}

		const AABB<V>& Bounds() override {
			//if (m_outdated) {
			//	m_bounds = AABB<V>(XYZ, D);
			//	m_outdated = false;
			//}

			return m_bounds;
		}
	};
}

	using PlaneCollider2  = impl::PlaneCollider<iw::vector2>;
	using PlaneCollider   = impl::PlaneCollider<iw::vector3>;
	using PlaneCollider4 = impl::PlaneCollider<iw::vector4>;
}

	using namespace Physics;
}
