#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/AABB.h"

namespace IW  {
inline namespace Physics {
	template<
		typename V>
	struct SphereCollider;

	template<
		typename V>
	struct Collider {
		AABB<V> Bounds;
		bool Outdated;

		IWPHYSICS_API
		virtual bool TestCollision(
			/*BoxCollider<V> */,
			V* resolve = nullptr) const = 0;

		IWPHYSICS_API
		virtual bool TestCollision(
			SphereCollider<V> other,
			V* resolve = nullptr) const = 0;

		IWPHYSICS_API
		virtual bool TestCollision(
			/*MeshColliderV */) const = 0;

		IWPHYSICS_API
		virtual AABB<V> GenAABB() const = 0;
	};
}
}
