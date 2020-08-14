#pragma once

#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT SphereCollider
		: Collider<V>
	{
		REFLECT V Center;
		REFLECT float Radius;

		SphereCollider()
			: Collider<V>(ColliderType::SPHERE)
			, Center(0.0f)
			, Radius(1.0f)
		{}

		SphereCollider(
			V center,
			float radius)
			: Collider<V>(ColliderType::SPHERE)
			, Center(center)
			, Radius(radius)
		{}

		const AABB<V>& Bounds() override {
			if (m_outdated) {
				m_bounds = AABB<V>(Center, Radius);
				m_outdated = false;
			}

			return m_bounds;
		}

		Transform Trans() const override {
			Transform transform;
			transform.Position = Center;
			transform.Scale    = Radius;

			return transform;
		}

		ManifoldPoints TestCollision(
			const Transform* transform,
			const Collider<V>* collider,
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

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const MeshCollider<V>* mesh,
			const Transform* meshTransform) const override;
	};
}

	using SphereCollider2 = impl::SphereCollider<iw::vector2>;
	using SphereCollider  = impl::SphereCollider<iw::vector3>;
	using SphereCollider4 = impl::SphereCollider<iw::vector4>;
}

	using namespace Physics;
}
