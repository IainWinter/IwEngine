#pragma once

#include "Collider.h"
#include "iw/math/vector3.h"
#include "iw/physics/Collision/CollisionMath/CollisionTests.h"
#include <type_traits>

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct BoxCollider
		: Collider<V>
	{

		V* Points;

		BoxCollider(
			V* points)
			: Points(points)
		{
			Bounds = AABB<V>(Center, scale);
		}

		bool TestCollision(
			const BoxCollider& other,
			V* resolve = nullptr) const override
		{
			return Algo::TestCollision(*this, other, resolve);
		}

		bool TestCollision(
			const SphereCollider& other,
			V* resolve = nullptr) const override
		{
			return Algo::TestCollision(*this, other, resolve);
		}

		AABB<V> GetAABB() const override {
			return new AABB<V>(Center, Scale);
		}

		void ExtrapolatePoints(
			V points
		)
		{

		}

	};
}

	using BoxCollider2 = impl::BoxCollider<iw::vector2>;
	using BoxCollider = impl::BoxCollider<iw::vector3>;
	using BoxCollider4 = impl::BoxCollider<iw::vector4>;
}

	using namespace Physics;

}
