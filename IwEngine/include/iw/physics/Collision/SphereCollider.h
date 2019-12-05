#pragma once

#include "Collider.h"
#include "iw/math/vector3.h"
#include "iw/physics/Collision/CollisionMath/CollisionTests.h"

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
				: Center(center)
				, Radius(radius)
			{}

			bool TestCollision(
				const SphereCollider& other,
				V* resolve = nullptr) const override
			{
				return Algo::TestCollision(*this, other, resolve);
			}

			//bool TestCollision(
			//	const BoxCollider<V>& other,
			//	V resolve = nullptr) const override
			//{
			//	return Algo::TestCollision(*this, other, resolve);
			//}

		};
}
}

inline namespace Physics {
	using CircleCollider  = impl::SphereCollider<iw::vector2>;
	using SphereCollider  = impl::SphereCollider<iw::vector3>;
	using SphereCollider4 = impl::SphereCollider<iw::vector4>;
}
}
