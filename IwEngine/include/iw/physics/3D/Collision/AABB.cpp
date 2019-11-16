#include "iw/physics/3D/Collision/AABB.h"

namespace IwPhysics {
	bool AABB::Intersects(
		const AABB& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y)
			&& (Min.z <= other.Max.z && Max.z >= other.Min.z);
	}

	bool AABB::Fits(
		const AABB& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x) 
			&& (Min.y < other.Min.y && Max.y > other.Max.y)
			&& (Min.z < other.Min.z && Max.z > other.Max.z);
	}
}
