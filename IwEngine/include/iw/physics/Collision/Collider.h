#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/AABB.h"

namespace IW  {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct SphereCollider;

	template<
		typename V>
	struct BoxCollider;
}
}

inline namespace Physics {
	template<
		typename V>
	struct Collider {
		AABB<V> Bounds;
		bool Outdated;

		IWPHYSICS_API
		virtual bool TestCollision(
			impl::BoxCollider<V> other,
			V* resolve = nullptr) const = 0;

		IWPHYSICS_API
		virtual bool TestCollision(
			impl::SphereCollider<V> other,
			V* resolve = nullptr) const = 0;

		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	/*MeshColliderV */) const = 0;

		IWPHYSICS_API
		virtual AABB<V> GetAABB() const = 0;
	};
}
}
