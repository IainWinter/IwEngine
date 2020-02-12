#include "iw/physics/AABB.h"

namespace iw {
namespace Physics {
	template<>
	inline bool AABB2::Intersects(
		const AABB2& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	template<>
	inline bool AABB::Intersects(
		const AABB& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y)
			&& (Min.z <= other.Max.z && Max.z >= other.Min.z);
	}

	template<>
	inline bool AABB4::Intersects(
		const AABB4& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y)
			&& (Min.z <= other.Max.z && Max.z >= other.Min.z)
			&& (Min.w <= other.Max.w && Max.w >= other.Min.w);
	}

	template<>
	inline bool AABB2::Fits(
		const AABB2& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y);
	}

	template<>
	inline bool AABB::Fits(
		const AABB& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y)
			&& (Min.z < other.Min.z && Max.z > other.Max.z);
	}

	template<>
	inline bool AABB4::Fits(
		const AABB4& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y)
			&& (Min.z < other.Min.z && Max.z > other.Max.z)
			&& (Min.w < other.Min.w && Max.w > other.Max.w);
	}
}
}
