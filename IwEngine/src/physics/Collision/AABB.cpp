#include "iw/physics/Collision/AABB.h"

namespace IwPhysics {
	iwm::vector3 AABB::Mid() const {
		return (Min + Max) / 2;
	}

	bool AABB::Intersects(
		const AABB& other) const
	{
		return (Min.x <= other.Max.x && Min.x >= other.Max.x) &&
			   (Min.y <= other.Max.y && Min.y >= other.Max.y) &&
			   (Min.z <= other.Max.z && Min.z >= other.Max.z);
	}

	bool AABB::Fits(
		const AABB& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x) &&
			   (Min.y < other.Min.y && Max.y > other.Max.y) &&
			   (Min.z < other.Min.z && Max.z > other.Max.z);
	}
}
