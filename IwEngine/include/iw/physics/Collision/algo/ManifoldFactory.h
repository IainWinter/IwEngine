#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/Manifold.h"

namespace IW {
namespace Physics {
namespace algo {
	IWPHYSICS_API
	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindSpherePlaneMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindPlaneSphereMaifoldPoints(
		const PlaneCollider*  a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);
}
}

	using namespace Physics;
}
