#pragma once

#include "IwPhysics.h"
#include "iw/log/logger.h"

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct AABB {
		V Min;
		V Max;

		AABB() = default;

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

	using AABB2 = impl::AABB<iw::vector2>;
	using AABB  = impl::AABB<iw::vector3>;
	using AABB4 = impl::AABB<iw::vector4>;
}

	using namespace Physics;
}
