#pragma once

#include "../IwPhysics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct REFLECT AABB {
		using vec_t = _vec<_d>;

		REFLECT vec_t Min;
		REFLECT vec_t Max;

		AABB() 
			: Min(FLT_MAX)
			, Max(FLT_MIN)
		{}

		AABB(
			const vec_t& p1,
			const vec_t& p2)
		{
			bool p1x = p1.x < p2.x;
			bool p1y = p1.y < p2.y;

			Min.x =  p1x ? p1.x : p2.x;
			Min.y =  p1y ? p1.y : p2.y;
			Max.x = !p1x ? p1.x : p2.x;
			Max.y = !p1y ? p1.y : p2.y;
		}

		AABB(
			const vec_t& center,
			float scale
		)
			: Min(center - scale)
			, Max(center + scale) // was / 2 before might be better
		{}

		vec_t Center() const {
			return (Min + Max) / scalar(2);
		}

		bool Intersects(
			                   const Transform* transform,
			const AABB& other, const Transform* otherTransform) const
		{
			vec_t tMin = Min * (vec_t)transform->WorldScale() + (vec_t)transform->WorldPosition(); // no rotation, maybe add function in Transform for this
			vec_t tMax = Max * (vec_t)transform->WorldScale() + (vec_t)transform->WorldPosition();
			
			vec_t toMin = other.Min * (vec_t)otherTransform->WorldScale() + (vec_t)otherTransform->WorldPosition();
			vec_t toMax = other.Max * (vec_t)otherTransform->WorldScale() + (vec_t)otherTransform->WorldPosition();

			for (size_t i = 0; i < (size_t)_d; i++) {
				if (tMin[i] > toMax[i] || tMax[i] < toMin[i]) return false;
			}

			return true;
		}

		bool Fits(
			                   const Transform* transform,
			const AABB& other, const Transform* otherTransform) const
		{
			vec_t tMin = Min * (vec_t)transform->WorldScale() + (vec_t)transform->WorldPosition();
			vec_t tMax = Max * (vec_t)transform->WorldScale() + (vec_t)transform->WorldPosition();

			vec_t toMin = other.Min * (vec_t)otherTransform->WorldScale() + (vec_t)otherTransform->WorldPosition();
			vec_t toMax = other.Max * (vec_t)otherTransform->WorldScale() + (vec_t)otherTransform->WorldPosition();

			for (size_t i = 0; i < (size_t)_d; i++) {
				if (tMin[i] >= toMin[i] || tMax[i] <= toMax[i]) return false;
			}

			return true;
		}
	};
}
}

	using namespace Physics;
}
