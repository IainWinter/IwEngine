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
			transform.Position = glm::vec3(Center);
			transform.Scale    = glm::vec3(Radius);

			return transform;
		}

		V FindFurthestPoint(
			const Transform* transform,
			V direction) const override
		{
			return Center + transform->WorldPosition()
				+ Radius * glm::normalize(direction) * major(transform->WorldScale());
		}
	};
}
}

	using namespace Physics;
}
