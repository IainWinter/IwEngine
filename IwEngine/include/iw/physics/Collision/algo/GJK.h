#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/CollisionObject.h"

namespace iw {
namespace Physics {
namespace algo {
	bool GJK(
		const CollisionObject* a,
		const CollisionObject* b);

namespace detail {
	iw::vector3 Support(
		const CollisionObject* a,
		const CollisionObject* b,
		const iw::vector3& direction);

	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b);

	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b,
		const iw::vector3& c);

	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b,
		const iw::vector3& c,
		const iw::vector3& d);

	bool SameDirection(
		const iw::vector3& direction,
		const iw::vector3& ao);
}
}
}

	using namespace Physics;
}
