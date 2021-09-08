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
			Bounds();
		}

		PlaneCollider(
			vec_t normal,
			float distance
		)
			: Collider<_d>(ColliderType::PLANE)
			, Normal(normal)
			, Distance(distance)
		{
			Bounds();
		}

		vec_t FindFurthestPoint(
			Transform* transform,
			const vec_t&     direction) const override
		{
			assert(false);
			return vec_t(0);
		}

		aabb_t CalcBounds() const
		{
			// could calc actual bounds but ill wait till
			// i need a non infinite plane

			return aabb_t();
		}

		bool CacheIsOld() const override
		{
			return  Normal   != t_normal
				|| Distance != t_distance;
		}

		void UpdateCache() override
		{
			t_normal   = Normal;
			t_distance = Distance;

			Collider<_d>::UpdateCache();
		}
	private:
		vec_t t_normal;
		float t_distance;
	};
}
}

	using namespace Physics;
}
