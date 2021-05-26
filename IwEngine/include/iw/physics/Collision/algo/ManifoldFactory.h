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
	// _ = no col

	// ________| Sphere  | Capsule | Plane  | Mesh |  Hull
	// Sphere  |    x    |    x    |    x   |   x  |   o
	// Capsule |         |    o    |    o   |   o  |   o
	// Plane   |         |         |    _   |   o  |   o
	// Mesh    |         |         |        |   x  |   o
	// Hull    |         |         |        |      |   _?

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const SphereCollider*  a, const Transform* ta,
		const CapsuleCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const PlaneCollider* a, const Transform* ta,
		const MeshCollider*  b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints FindGJKMaifoldPoints(
		const Collider* a, const Transform* ta,
		const Collider* b, const Transform* tb);

	inline ManifoldPoints TestCollision(
		const MeshCollider* a, const Transform* ta,
		const MeshCollider* b, const Transform* tb)
	{
		return FindGJKMaifoldPoints(a, ta, b, tb);
	}
	
	// Swaps

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const PlaneCollider*  a, const Transform* ta,
		const SphereCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const MeshCollider*  a, const Transform* ta,
		const PlaneCollider* b, const Transform* tb);

	IWPHYSICS_API
	ManifoldPoints TestCollision(
		const CapsuleCollider* a, const Transform* ta,
		const SphereCollider*  b, const Transform* tb);

	// Trash

	inline ManifoldPoints TestCollision(const CapsuleCollider* a, const Transform* ta, const CapsuleCollider* b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const CapsuleCollider* a, const Transform* ta, const PlaneCollider*   b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const PlaneCollider*   a, const Transform* ta, const PlaneCollider*   b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const PlaneCollider*   a, const Transform* ta, const CapsuleCollider* b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const SphereCollider*  a, const Transform* ta, const MeshCollider*    b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const MeshCollider*    a, const Transform* ta, const SphereCollider*  b, const Transform* tb) { return {}; }
	inline ManifoldPoints TestCollision(const CapsuleCollider* a, const Transform* ta, const MeshCollider*    b, const Transform* tb) {return {};}
	inline ManifoldPoints TestCollision(const MeshCollider*    a, const Transform* ta, const CapsuleCollider* b, const Transform* tb) {return {};}
}

inline ManifoldPoints TestCollision(
		const Collider* a, const Transform* ta,
		const Collider* b, const Transform* tb)
	{
		//bool sort = a->Type() > b->Type();

		//const Collider* A = sort ? b : a; // A is least
		//const Collider* B = sort ? a : b;

		//const Transform* tA = sort ? tb : ta;
		//const Transform* tB = sort ? ta : tb;

		if (     (a->Type() == ColliderType::MESH  || b->Type() == ColliderType::MESH)
			&& !(a->Type() == ColliderType::PLANE || b->Type() == ColliderType::PLANE))
		{
			return algo::FindGJKMaifoldPoints(a, ta, b, tb);
		}

#define FIND_TYPE(x, X, E)\
		switch (x->Type()) {\
			case ColliderType::SPHERE:{\
				SphereCollider* X = (SphereCollider*)x;\
				E\
				break;}\
			case ColliderType::CAPSULE:{\
				CapsuleCollider* X = (CapsuleCollider*)x;\
				E\
				break;}\
			case ColliderType::PLANE:{\
				PlaneCollider* X = (PlaneCollider*)x;\
				E\
				break;}\
			case ColliderType::MESH:{\
				MeshCollider* X = (MeshCollider*)x;\
				E\
				break;}\
		}\

		FIND_TYPE(a, A, FIND_TYPE(b, B, return algo::TestCollision(A, ta, B, tb);))
	}
}

			//case ColliderType::CYLINDER:{\
			//	CylinderCollider* X = (CylinderCollider*)x;\
			//	E\
			//	break;}\


	using namespace Physics;
}
