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
			: Collider<V>(ColliderType::MESH)
		{}

		static MeshCollider MakeCube() {
			MeshCollider collider;
			collider.m_points.push_back(V(-1, -1, -1)); // 0 
			collider.m_points.push_back(V(-1,  1, -1)); // 1 
			collider.m_points.push_back(V( 1,  1, -1)); // 2 
			collider.m_points.push_back(V( 1, -1, -1)); // 3 

			collider.m_points.push_back(V(-1, -1,  1)); // 7
			collider.m_points.push_back(V( 1, -1,  1)); // 6
			collider.m_points.push_back(V( 1,  1,  1)); // 5
			collider.m_points.push_back(V(-1,  1,  1)); // 4

			return collider;
		}

		//IWPHYSICS_API
		const AABB<V>& Bounds() { return AABB<V>(); }

		//IWPHYSICS_API
		Transform Trans() const { return Transform(); }

		IWPHYSICS_API
		V FindFurthestPoint(
			const Transform* transform,
			V direction) const override;

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
			const impl::SphereCollider<V>* sphere,
			const Transform* sphereTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::CapsuleCollider<V>* capsule,
			const Transform* capsuleTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::PlaneCollider<V>* plane,
			const Transform* planeTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::MeshCollider<V>* mesh,
			const Transform* meshTransform) const override;
	};
}

	using MeshCollider2 = impl::MeshCollider<iw::vector2>;
	using MeshCollider  = impl::MeshCollider<iw::vector3>;
	using MeshCollider4 = impl::MeshCollider<iw::vector4>;
}

	using namespace Physics;
}
