#pragma once

#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct REFLECT SphereCollider
		: Collider<_d>
	{
		using vec_t = _vec<_d>;
		using aabb_t = AABB<_d>;

		REFLECT vec_t Center;
		REFLECT float Radius;

	public:
		SphereCollider()
			: Collider<_d>(ColliderType::SPHERE)
			, Center(0.0f)
			, Radius(1.0f)
		{
			Bounds();
		}

		SphereCollider(
			vec_t center,
			float radius
		)
			: Collider<_d>(ColliderType::SPHERE)
			, Center(center)
			, Radius(radius)
		{
			Bounds();
		}

		vec_t FindFurthestPoint(
			Transform* transform,
			const vec_t&     direction) const override
		{
			return Center + (vec_t)transform->WorldPosition()
				+ Radius * normalize(direction) * major(transform->WorldScale());
		}

		aabb_t CalcBounds() const
		{
			return aabb_t(Center, sqrt(Radius*Radius + Radius*Radius));
		}

		bool CacheIsOld() const override
		{
			return  Center != t_center
				|| Radius != t_radius;
		}

		void UpdateCache() override
		{
			t_center = Center;
			t_radius = Radius;

			Collider<_d>::UpdateCache();
		}
	private:
		vec_t t_center;
		float t_radius;
	};
}
}

	using namespace Physics;
}
