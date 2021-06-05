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
			const vec_t& min,
			const vec_t& max
		)
			: Min(min)
			, Max(max)
		{}

		AABB(
			const vec_t& center,
			float scale
		)
			: Min(center - scale)
			, Max(center + scale) // was / 2 before might be better
		{}

		vec_t Center() const {
			return (Min + Max) / 2;
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
