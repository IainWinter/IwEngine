#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/Manifold.h"
#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
namespace Physics {
namespace algo {
	IWPHYSICS_API
	Manifold MakeManifold(
		const Rigidbody* a,
		const Rigidbody* b);

namespace detail {
	struct ManifoldPoints {
		iw::vector3 A;
		iw::vector3 B;
		bool NoContact;
	};

	ManifoldPoints FindManifoldPoints(
		const CollisionObject* a,
		const CollisionObject* b);

	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);
}
}
}

	using namespace Physics;
}
