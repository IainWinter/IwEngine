#include "iw/physics/Collision/AABB.h"

namespace IwPhysics {
	AABB::AABB(
		iwm::vector3 center,
		float scale)
		: Min(center - scale / 2)
		, Max(center + scale / 2)
	{}

	AABB::AABB(
		iwm::vector3 min, 
		iwm::vector3 max)
		: Min(min)
		, Max(max)
	{}

	iwm::vector3 AABB::Mid() const {
		return (Min + Max) / 2;
	}

	bool AABB::Intersects(
		const AABB& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x) &&
			   (Min.y <= other.Max.y && Max.y >= other.Min.y) &&
			   (Min.z <= other.Max.z && Max.z >= other.Min.z);
	}

	bool AABB::Fits(
		const AABB& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x) &&
			   (Min.y < other.Min.y && Max.y > other.Max.y) &&
			   (Min.z < other.Min.z && Max.z > other.Max.z);
	}
}
