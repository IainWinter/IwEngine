#pragma once

#include "Collider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT CapsuleCollider
		: Collider<V>
	{
		REFLECT V Center;
		REFLECT V Direction;
		REFLECT float Height;
		REFLECT float Radius;

		CapsuleCollider()
			: Collider<V>(ColliderType::CAPSULE)
			, Center(0.0f)
			, Direction(V::unit_y)
			, Height(2.0f)
			, Radius(0.5f)
		{}

		CapsuleCollider(
			V center,
			float height,
			float radius)
			: Collider<V>(ColliderType::CAPSULE)
			, Center(position)
			, Direction(V::unit_y)
			, Height(height)
			, Radius(radius)
		{}

		const AABB<V>& Bounds() override {
			if (m_outdated) {
				m_bounds = AABB<V>(
					/*Position + Position.normalized() * Radius,
					Position + Offset + Offset.normalized() * Radius*/
				);

				m_outdated = false;
			}

			return m_bounds;
		}

		Transform Trans() const override {
			Transform transform;
			transform.Position = Center;
			//transform.Scale    = vector3(Radius, Height / 2, Radius);

			if      (Direction == V::unit_x) {
				transform.Rotation = quaternion::from_axis_angle(V::unit_z, Pi / 2);
			}

			else if (Direction == V::unit_y) {
				transform.Rotation = quaternion::identity;
			}

			else if (Direction == V::unit_z) {
				transform.Rotation = quaternion::from_axis_angle(V::unit_x, Pi / 2);
			}

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
	};
}

	using CapsuleCollider2 = impl::CapsuleCollider<iw::vector2>;
	using CapsuleCollider  = impl::CapsuleCollider<iw::vector3>;
	using CapsuleCollider4 = impl::CapsuleCollider<iw::vector4>;
}

	using namespace Physics;
}
