#pragma once

#include "iw/physics/Collision/ManifoldPoints.h"
#include "CollisionObject.h"

namespace iw {
namespace Physics {
	struct Manifold {
		CollisionObject* ObjA;
		CollisionObject* ObjB;
		glm::vec3 A;
		glm::vec3 B;
		glm::vec3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;

		Manifold(
			CollisionObject* a,
			CollisionObject* b,
			const ManifoldPoints& points)
			: ObjA(a)
			, ObjB(b)
			, A(points.A)
			, B(points.B)
			, Normal(points.Normal)
			, PenetrationDepth(points.PenetrationDepth)
			, HasCollision(points.HasCollision)
		{}
	};
}

	using namespace Physics;
}
