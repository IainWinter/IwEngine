#pragma once

#include "Collider.h"
#include <vector>
#include <type_traits>

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct MeshCollider
		: Collider<V>
	{
	private:
		std::vector<V> m_points;

	public:
		MeshCollider()
			: Collider<V>()
		{}

		static MeshCollider MakeCube() {
			MeshCollider collider;
			collider.m_points.push_back(V(-1, -1, -1));
			collider.m_points.push_back(V( 1, -1, -1));
			collider.m_points.push_back(V( 1,  1, -1));
			collider.m_points.push_back(V(-1,  1, -1));
			collider.m_points.push_back(V(-1,  1,  1));
			collider.m_points.push_back(V( 1,  1,  1));
			collider.m_points.push_back(V( 1, -1,  1));
			collider.m_points.push_back(V(-1, -1,  1));

			return collider;
		}

		V FindFurthestPoint(
			V direction) const override
		{
			V*    maxPoint;
			float maxDistance = FLT_MIN;

			for (V point : m_points) {
				float distance = point.dot(direction);
				if (distance > maxDistance) {
					maxDistance = distance;
					maxPoints   = point;
				}
			}

			return maxPoint;
		}

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::Collider<V>* collider,
			const Transform* colliderTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::SphereCollider<V>* sphere,
			const Transform* sphereTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::CapsuleCollider<V>* capsule,
			const Transform* capsuleTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::PlaneCollider<V>* plane,
			const Transform* planeTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::MeshCollider<V>* mesh,
			const Transform* meshTransform) const = 0;
	};
}

	using MeshCollider2 = impl::MeshCollider<iw::vector2>;
	using MeshCollider  = impl::MeshCollider<iw::vector3>;
	using MeshCollider4 = impl::MeshCollider<iw::vector4>;
}

	using namespace Physics;
}
