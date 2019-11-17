#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/Collider.h"
#include "iw/physics/Collision/CollisionData.h"

namespace IwPhysics {
	bool GJK(
		const Collider& collider1, 
		const Collider& collider2,
		const iw::matrix4& transformation1,
		const iw::matrix4& transformation2);

	iw::vector3 Support(
		const Collider& collider1,
		const Collider& collider2,
		const iw::matrix4& transformation1,
		const iw::matrix4& transformation2,
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

	inline bool SameDirection(
		const iw::vector3& direction,
		const iw::vector3& ao)
	{
		return direction.dot(ao) > 0;
	}
}
