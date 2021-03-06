#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/Collision/Manifold.h"

namespace iw {
namespace Physics {
namespace algo {
	// x = col impl
	// o = col no impl
	//   = no col

	// ________| Sphere  | Capsule | Plane  | Mesh
	// Sphere  |    x    |    x    |    x   |   x
	// Capsule |    x    |    o    |    o   |   o
	// Plane   |    x    |    o    |        |   o
	// Mesh    |    o    |    o    |    o   |   x

	IWPHYSICS_API
	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindSpherePlaneMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindSphereCapsuleMaifoldPoints(
		const SphereCollider*  a, const Transform* ta,
		const CapsuleCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindPlaneMeshMaifoldPoints(
		const PlaneCollider* a, const Transform* ta,
		const MeshCollider*  b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindGJKMaifoldPoints(
		const Collider* a, const Transform* ta,
		const Collider* b, const Transform* tb);


	// Swaps

	IWPHYSICS_API
	ManifoldPoints FindPlaneSphereMaifoldPoints(
		const PlaneCollider*  a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindMeshPlaneMaifoldPoints(
		const MeshCollider*  a, const Transform* ta,
		const PlaneCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindCapsuleSphereMaifoldPoints(
		const CapsuleCollider* a, const Transform* ta,
		const SphereCollider*  b, const Transform* tb);
}
}

	using namespace Physics;
}
