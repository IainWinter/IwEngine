#pragma once

#include "IwPhysics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT AABB {
		REFLECT V Min;
		REFLECT V Max;

		AABB() 
			: Min(FLT_MAX)
			, Max(FLT_MIN)
		{}

		AABB(
			const V& min,
			const V& max)
			: Min(min)
			, Max(max)
		{}

		AABB(
			const V& center,
			float scale)
			: Min(center - scale)
			, Max(center + scale) // was / 2 before might be better
		{}

		inline V Center() const {
			return (Min + Max) / 2;
		}

		IWPHYSICS_API
		bool Intersects(
			const Transform* transform,
			const AABB& other,
			const Transform* otherTransform) const
		{
			V tMin = Min * transform->WorldScale() + transform->WorldPosition(); // no rotation, maybe add function in Transform for this
			V tMax = Max * transform->WorldScale() + transform->WorldPosition();

			V toMin = other.Min * otherTransform->WorldScale() + otherTransform->WorldPosition();
			V toMax = other.Max * otherTransform->WorldScale() + otherTransform->WorldPosition();

			for (size_t i = 0; i < V::length(); i++) {
				if (tMin[i] > toMax[i] || tMax[i] < toMin[i]) return false;
			}

			return true;
		}

		IWPHYSICS_API
		bool Fits(
			const Transform* transform,
			const AABB& other,
			const Transform* otherTransform) const
		{
			V tMin = Min * transform->WorldScale() + transform->WorldPosition();
			V tMax = Max * transform->WorldScale() + transform->WorldPosition();

			V toMin = other.Min * otherTransform->WorldScale() + otherTransform->WorldPosition();
			V toMax = other.Max * otherTransform->WorldScale() + otherTransform->WorldPosition();

			for (size_t i = 0; i < V::length(); i++) {
				if (tMin[i] >= toMin[i] || tMax[i] <= toMax[i]) return false;
			}

			return true;
		}
	};
}

	using AABB2 = impl::AABB<glm::vec2>;
	using AABB  = impl::AABB<glm::vec3>;
	using AABB4 = impl::AABB<glm::vec4>;
}

	using namespace Physics;
}
