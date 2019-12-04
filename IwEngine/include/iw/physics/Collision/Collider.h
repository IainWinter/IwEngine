#pragma once

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
			BoxCollider<V> ) const = 0;

		IWPHYSICS_API
		virtual bool TestCollision(
			SphereCollider<V> other,
			V* resolve = nullptr) const = 0;

		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	/*MeshColliderV */) const = 0;

		IWPHYSICS_API
		virtual AABB<V> GetAABB() const = 0;
	};
}
}
