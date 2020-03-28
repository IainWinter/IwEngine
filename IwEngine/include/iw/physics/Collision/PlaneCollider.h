#pragma once

#include "Collider.h"
#include "iw/physics/Plane.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT PlaneCollider
		: Collider<V>
	{
		REFLECT impl::Plane<V> Plane;

		PlaneCollider()
			: Plane(0.0f, 0.0f)
		{}

		PlaneCollider(
			V p,
			float d)
			: Collider<V>()
			, Plane(p, d)
		{}

		PlaneCollider(
			impl::Plane<V> plane)
			: Collider<V>()
			, Plane(plane)
		{}

		const AABB<V>& Bounds() override {
			//if (m_outdated) {
			//	m_bounds = AABB<V>(XYZ, D);
			//	m_outdated = false;
			//}

			return m_bounds;
		}

		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::Collider<V>* collider,
			const Transform* colliderTransform) const override
		{
			return collider->TestCollision(colliderTransform, this, transform);
		}

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const SphereCollider<V>* sphere,
			const Transform* sphereTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const CapsuleCollider<V>* capsule,
			const Transform* capsuleTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const PlaneCollider<V>* plane,
			const Transform* planeTransform) const override;
	};
}

	using PlaneCollider2 = impl::PlaneCollider<iw::vector2>;
	using PlaneCollider  = impl::PlaneCollider<iw::vector3>;
	using PlaneCollider4 = impl::PlaneCollider<iw::vector4>;
}

	using namespace Physics;
}
