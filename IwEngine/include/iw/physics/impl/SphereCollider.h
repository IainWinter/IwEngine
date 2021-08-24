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

		SphereCollider()
			: Collider<_d>(ColliderType::SPHERE)
			, Center(0.0f)
			, Radius(1.0f)
		{}

		SphereCollider(
			vec_t center,
			float radius
		)
			: Collider<_d>(ColliderType::SPHERE)
			, Center(center)
			, Radius(radius)
		{}

		vec_t FindFurthestPoint(
			const Transform* transform,
			const vec_t&     direction) const override
		{
			return Center + (vec_t)transform->WorldPosition()
				+ Radius * normalize(direction) * major(transform->WorldScale());
		}
	protected:
		aabb_t GenerateBounds() const override {
			return aabb_t(Center, sqrt(Radius*Radius + Radius*Radius));
		}
	};
}
}

	using namespace Physics;
}
