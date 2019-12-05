#pragma once
#include "Collider.h"
#include "iw/math/vector3.h"
#include "iw/physics/Collision/CollisionMath/CollisionTests.h"

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct BoxCollider
		: Collider<V>
	{
		
		V Center;
		float Scale;

		BoxCollider(
			V center,
			float scale)
			: Center(center)
			, Scale(scale)
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
	};
}
}
inline namespace Physics {
	using BoxCollider2 = impl::BoxCollider<iw::vector2>;
	using BoxCollider = impl::BoxCollider<iw::vector3>;
	using BoxCollider4  = impl::BoxCollider<iw::vector4>;
}
}
