#pragma once
#include "Collider.h"
#include "iw/math/vector3.h"
#include "iw/physics/Collision/CollisionMath/CollisionTests.h"

namespace IW {
inline namespace Physics {
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
