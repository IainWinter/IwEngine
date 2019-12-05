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

	template<
		typename V>
	struct Collider {
		impl::AABB<V> Bounds;
		bool Outdated;

		IWPHYSICS_API
		virtual bool TestCollision(
			IW::impl::BoxCollider<V> other,
			V* resolve = nullptr) const = 0;

		IWPHYSICS_API
		virtual bool TestCollision(
			IW::impl::SphereCollider<V> other,
			V* resolve = nullptr) const = 0;

		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	/*MeshColliderV */) const = 0;

		IWPHYSICS_API
		virtual IW::impl::AABB<V> GetAABB() const = 0;
	};
}

	using Collider2 = impl::Collider<iw::vector2>;
	using Collider  = impl::Collider<iw::vector3>;
	using Collider4 = impl::Collider<iw::vector4>;
}

	using namespace Physics;
}
