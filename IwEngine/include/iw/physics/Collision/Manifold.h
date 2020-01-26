#pragma once

#include "iw/physics/Collision/ManifoldPoints.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace IW {
namespace Physics {
	struct Manifold {
		Rigidbody* BodyA;
		Rigidbody* BodyB;
		iw::vector3 A;
		iw::vector3 B;
		iw::vector3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;

		Manifold(
			Rigidbody* a,
			Rigidbody* b,
			const ManifoldPoints& points)
			: BodyA(a)
			, BodyB(b)
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
