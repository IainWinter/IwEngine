#pragma once

#include "IwPhysics.h"

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
			const AABB& other) const;

		IWPHYSICS_API
		bool Fits(
			const AABB& other) const;
	};
}

	using AABB2 = impl::AABB<glm::vec2>;
	using AABB  = impl::AABB<glm::vec3>;
	using AABB4 = impl::AABB<glm::vec4>;
}

	using namespace Physics;
}
