#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/Collider.h"
#include "iw/physics/Collision/CollisionData.h"

namespace IwPhysics {
	bool GJK(
		const Collider& collider1, 
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2);

	iwm::vector3 Support(
		const Collider& collider1,
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2,
		const iwm::vector3& direction);

	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b);

	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b,
		const iwm::vector3& c);

	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b,
		const iwm::vector3& c,
		const iwm::vector3& d);

	inline bool SameDirection(
		const iwmath::vector3& direction,
		const iwmath::vector3& ao)
	{
		return direction.dot(ao) > 0;
	}
}
