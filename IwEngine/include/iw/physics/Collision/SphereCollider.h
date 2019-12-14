#pragma once

#include "Collider.h"
#include "iw/math/vector3.h"

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct SphereCollider
		: Collider<V>
	{
		V Center;
		float Radius;

		SphereCollider(
			V center,
			float radius)
			: Collider<V>(ColliderShape::SPHERE)
			, Center(center)
			, Radius(radius)
		{}

		const AABB<V>& Bounds() override {
			if (m_outdated) {
				m_bounds = AABB<V>(Center, Radius);
				m_outdated = false;
			}

			return m_bounds;
		}
	};
}

	using SphereCollider2 = impl::SphereCollider<iw::vector2>;
	using SphereCollider  = impl::SphereCollider<iw::vector3>;
	using SphereCollider4 = impl::SphereCollider<iw::vector4>;
}

	using namespace Physics;
}
