#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/Collider.h"
#include "iw/physics/Collision/CollisionData.h"
#include "iw/math/vector3.h"
#include "iw/math/matrix4.h"

namespace IwPhysics {
	CollisionData GJK(
		const Collider& collider1,
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2);

	// Furthest point in a direction
	iwm::vector3 Support(
		const Collider& collider1,
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2,
		const iwm::vector3& direction);

	// Line
	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b);

	// Triangle
	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b,
		const iwm::vector3& c);

	// Trapezoid
	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b,
		const iwm::vector3& c,
		const iwm::vector3& d);
}
