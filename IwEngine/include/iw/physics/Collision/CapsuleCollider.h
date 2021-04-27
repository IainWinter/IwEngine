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
			, Center(center)
			, Direction(V::unit_y)
			, Height(height)
			, Radius(radius)
		{}

		CapsuleCollider(
			V center,
			V direction,
			float height,
			float radius)
			: Collider<V>(ColliderType::CAPSULE)
			, Center(center)
			, Direction(direction)
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
			//transform.Scale    = glm::vec3(Radius, Height / 2, Radius); // todo: why would this be commented?

			if      (Direction == glm::vec3(1, 0, 0)) {
				transform.Rotation = glm::angleAxis(glm::pi<float>() * 0.5f, glm::vec3(0, 0, 1));
			}

			else if (Direction == glm::vec3(0, 1, 0)) {
				transform.Rotation = glm::quat();
			}

			else if (Direction == glm::vec3(0, 0, 1)) {
				transform.Rotation = glm::angleAxis(glm::pi<float>() * 0.5f, glm::vec3(1, 0, 0));
			}

			return transform;
		}
	};
}
}

	using namespace Physics;
}
