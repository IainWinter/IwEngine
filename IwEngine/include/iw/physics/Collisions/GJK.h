#pragma once

#include "iw/physics/iwphysics.h"
#include "IwPhysics\Collision\collider.h"
#include "iw/math/vector3.h"

namespace iwphysics {
	struct collision_transformation {
		const transformation* transformation1;
		const transformation* transformation2;

		collision_transformation(const transformation* transformation1, const transformation* transformation2)
			: transformation1(transformation1), transformation2(transformation2) {}
	};

	struct collision_data {
		const collider* colliderA;
		const collider* colliderB;
		iwmath::vector3 point_of_impact;

		collision_data(const collider* colliderA, const collider* colliderB, iwmath::vector3 point_of_impact)
			: colliderA(colliderA), colliderB(colliderB), point_of_impact(point_of_impact) {}
	};

	collision_data* GJK(
		const collider* bounds1,
		const collider* bounds2,
		const collision_transformation& collisionTrans);

	bool is_same_direction(const iwmath::vector3& direction, const iwmath::vector3& ao) {
		return direction.dot(ao) > 0;
	}

	//Furthest point in a direction
	iwmath::vector3 support(const collider* bounds1, const collider* bounds2,
		const collision_transformation& collisionTrans, const iwmath::vector3& direction);

	//Line
	iwmath::vector3 simplex(const iwmath::vector3& a, const iwmath::vector3& b);

	//Triangle
	iwmath::vector3 simplex(const iwmath::vector3& a, const iwmath::vector3& b,
		const iwmath::vector3& c);

	//Trapazoid
	iwmath::vector3 simplex(const iwmath::vector3& a, const iwmath::vector3& b,
		const iwmath::vector3& c, const iwmath::vector3& d);

}
