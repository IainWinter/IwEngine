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
		REFLECT float Radius;
		REFLECT float Height;

		CapsuleCollider()
			: Collider<_d>(ColliderType::CAPSULE)
			, Center(0.0f)
			, Height(2.0f)
			, Radius(0.5f)
		{
			Bounds();
		}

		CapsuleCollider(
			vec_t center,
			float height,
			float radius
		)
			: Collider<_d>(ColliderType::CAPSULE)
			, Center(center)
			, Height(height)
			, Radius(radius)
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

		// todo: check if this is correct
		aabb_t CalcBounds() const
		{
			vec_t y(0); y[1] = 1;

			vec_t a = Center + y * (Height / 2) + vec_t(1) * Radius;
			vec_t b = Center - y * (Height / 2) - vec_t(1) * Radius;

			return aabb_t(a, b);
		}

		bool CacheIsOld() const override
		{
			return  Center != t_center
				|| Radius != t_radius
				|| Height != t_height;
		}

		void UpdateCache()
		{
			t_center = Center;
			t_radius = Radius;
			t_height = Height;
		}
	private:
		vec_t t_center;
		float t_radius;
		float t_height;
	};
}
}

	using namespace Physics;
}
