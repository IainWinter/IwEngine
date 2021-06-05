#pragma once

#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct REFLECT PlaneCollider
		: Collider<_d>
	{
		using vec_t = _vec<_d>;
		using aabb_t = AABB<_d>;

		REFLECT vec_t Normal;
		REFLECT float Distance;

		PlaneCollider()
			: Collider<_d>(ColliderType::PLANE)
			, Normal(0)
			, Distance(0)
		{
			Normal[1] = 1; // y = 1 as default
		}

		PlaneCollider(
			vec_t normal,
			float distance
		)
			: Collider<_d>(ColliderType::PLANE)
			, Normal(normal)
			, Distance(distance)
		{}

		vec_t FindFurthestPoint(
			const Transform* transform,
			const vec_t&     direction) const override
		{
			assert(false);
			return vec_t(0);
		}

	protected:
		aabb_t GenerateBounds() const override {
			assert(false);
			return aabb_t(); // todo: impl maybe
		}
	};
}
}

	using namespace Physics;
}
