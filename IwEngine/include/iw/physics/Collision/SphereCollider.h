#pragma once

#include "Collider.h"
#include "CollisionMath/CollisionTests.h"
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
			: Collider<V>()
			, Center(center)
			, Radius(radius)
		{}

		//bool TestCollision(
		//	const SphereCollider& other,
		//	V* resolve = nullptr) const override
		//{
		//	return algo::TestCollision(*this, other, resolve);
		//}

		//bool TestCollision(
		//	const BoxCollider<V>& other,
		//	V resolve = nullptr) const override
		//{
		//	return Algo::TestCollision(*this, other, resolve);
		//}

				//void Transform(
		//	const IW::Transform& transform) override;

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
