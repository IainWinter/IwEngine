#pragma once

#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct REFLECT CapsuleCollider
		: Collider<_d>
	{
		using vec_t = _vec<_d>;
		using aabb_t = AABB<_d>;

		REFLECT vec_t Center;
		REFLECT float Height;
		REFLECT float Radius;

		CapsuleCollider()
			: Collider<_d>(ColliderType::CAPSULE)
			, Center(0.0f)
			, Height(2.0f)
			, Radius(0.5f)
		{}

		CapsuleCollider(
			vec_t center,
			float height,
			float radius
		)
			: Collider<_d>(ColliderType::CAPSULE)
			, Center(center)
			, Height(height)
			, Radius(radius)
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
			vec_t y(0); y[1] = 1;

			vec_t a = Center + y * (Height / 2) + vec_t(1) * Radius; // todo: check if this is correct
			vec_t b = Center - y * (Height / 2) - vec_t(1) * Radius;

			return aabb_t(a, b);
		}
	};
}
}

	using namespace Physics;
}
