#include "iw/physics/2D/Collision/AABB2D.h"

namespace IwPhysics {
	AABB2D::AABB2D(
		iwm::vector2 center,
		float scale)
		: Min(center - scale / 2)
		, Max(center + scale / 2)
	{}

	AABB2D::AABB2D(
		iwm::vector2 min,
		iwm::vector2 max)
		: Min(min)
		, Max(max)
	{}

	iwm::vector2 AABB2D::Mid() const {
		return (Min + Max) / 2;
	}

	bool AABB2D::Intersects(
		const AABB2D& other) const
	{
		return (Min.x <= other.Max.x && Max.x >= other.Min.x)
			&& (Min.y <= other.Max.y && Max.y >= other.Min.y);
	}

	bool AABB2D::Fits(
		const AABB2D& other) const
	{
		return (Min.x < other.Min.x && Max.x > other.Max.x)
			&& (Min.y < other.Min.y && Max.y > other.Max.y);
	}
}
