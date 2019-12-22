#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/Manifold.h"
#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
namespace Physics {
namespace algo {
	IWPHYSICS_API
	Manifold MakeManifold(
		Rigidbody* a,
		Rigidbody* b);

namespace detail {
	struct ManifoldPoints {
		iw::vector3 A;
		iw::vector3 B;
		bool NoContact;
	};

	ManifoldPoints FindManifoldPoints(
		CollisionObject* a,
		CollisionObject* b);

	ManifoldPoints FindSphereSphereMaifoldPoints(
		SphereCollider* a, Transform* ta,
		SphereCollider* b, Transform* tb);

	ManifoldPoints FindSpherePlaneMaifoldPoints(
		SphereCollider* a, Transform* ta,
		PlaneCollider* b, Transform* tb);
}
}
}

	using namespace Physics;
}
