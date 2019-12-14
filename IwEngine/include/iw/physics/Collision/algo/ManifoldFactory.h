#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/Manifold.h"
#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
namespace Physics {
namespace algo {
	Manifold MakeManifold(
		const Rigidbody* a,
		const Rigidbody* b);

namespace detail {
	struct ManifoldPoints {
		iw::vector3 A;
		iw::vector3 B;
	};

	ManifoldPoints FindManifoldPoints(
		const CollisionObject* a,
		const CollisionObject* b);

	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a,
		const SphereCollider* b);
}
}
}

	using namespace Physics;
}
