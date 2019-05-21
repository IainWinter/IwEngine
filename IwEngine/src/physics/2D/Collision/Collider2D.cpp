#include "iw/physics/2D/Collision/Collider2D.h"

namespace IwPhysics {
	Collider2D::Collider2D(
		const AABB2D& bounds)
		: Bounds(bounds)
	{}
}
