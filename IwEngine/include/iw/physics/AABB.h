#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include "iw/log/logger.h"

namespace IwPhysics {
	template<
		typename _v>
	struct AABB {
		_v Min;
		_v Max;

		AABB() = default;

		AABB(
			const _v& min,
			const _v& max)
			: Min(min)
			, Max(max)
		{}

		AABB(
			const _v& center,
			float scale)
			: Min(center - scale / 2)
			, Max(center + scale / 2)
		{}

		_v Center() const {
			return (Min + Max) / 2;
		}

		bool Intersects(
			const AABB& other) const;

		bool Fits(
			const AABB& other) const;
	};

	using AABB2D = AABB<iw::vector2>;
	using AABB3D = AABB<iw::vector3>;
	using AABB4D = AABB<iw::vector4>;

	template<>
	inline bool AABB<iw::vector2>::Intersects(
		const AABB& other) const
	{
		return  (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	template<>
	inline bool AABB<iw::vector3>::Intersects(
		const AABB& other) const
	{
		return  (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y)
			&& (Min.z <= other.Max.z && Max.z >= other.Min.z);
	}

	template<>
	inline bool AABB<iw::vector4>::Intersects(
		const AABB& other) const
	{
		return  (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y)
			&& (Min.z <= other.Max.z && Max.z >= other.Min.z)
			&& (Min.w <= other.Max.w && Max.w >= other.Min.w);
	}

	template<>
	inline bool AABB<iw::vector2>::Fits(
		const AABB& other) const
	{
		return  (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y);
	}

	template<>
	inline bool AABB<iw::vector3>::Fits(
		const AABB& other) const
	{
		return  (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y)
			&& (Min.z < other.Min.z && Max.z > other.Max.z);
	}

	template<>
	inline bool AABB<iw::vector4>::Fits(
		const AABB& other) const
	{
		return  (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y)
			&& (Min.z < other.Min.z && Max.z > other.Max.z)
			&& (Min.w < other.Min.w && Max.w > other.Max.w);
	}
}
