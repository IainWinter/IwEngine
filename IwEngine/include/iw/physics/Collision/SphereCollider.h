#pragma once
#include "Collider.h"
#include "iw/math/vector3.h"

namespace IW {
inline namespace Physics {
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
			: Center(center)
			, Radius(radius)
		{}
		
		bool TestCollision(
			const SphereCollider& other,
			V* resolve = nullptr) const override
		{
			return Algo::TestCollision(*this, other, resolve);
		}

	};
}
}
