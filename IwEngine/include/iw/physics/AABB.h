#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
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
			: Min(center - scale / 2)
			, Max(center + scale / 2)
		{}

		inline V Center() const {
			return (Min + Max) / 2;
		}

		bool Intersects(
			const AABB& other) const;

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
