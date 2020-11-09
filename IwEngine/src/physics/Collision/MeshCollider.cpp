#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
	template<>
	vector3 MeshCollider::FindFurthestPoint(
		const Transform* transform,
		vector3 direction) const
	{
		//vector4 dir = vector4(direction, 1);
		//dir *= transform->WorldTransformation().inverted(); // I think we can transform the direction instead of the collider here

		//direction = dir.xyz();

		vector3 maxPoint;
		float   maxDistance = -FLT_MAX;

		for (vector3 point : m_points) {
			point = vector3(vector4(point, 1) * transform->WorldTransformation().transposed());

			float distance = point.dot(direction);
			if (distance > maxDistance) {
				maxDistance = distance;
				maxPoint = point;
			}
		}

		//vector4 mp = vector4(maxPoint, 1);  // and then transform the final point?
		//mp *= transform->WorldTransformation();

		return maxPoint;//mp.xyz();
	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		return algo::FindGJKMaifoldPoints(
			this, transform, sphere, sphereTransform);
	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::CapsuleCollider* capsule,
		const Transform* capsuleTransform) const
	{
		return algo::FindGJKMaifoldPoints(
			this, transform, capsule, capsuleTransform);
	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return algo::FindGJKMaifoldPoints(
			this, transform, plane, planeTransform);
	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const MeshCollider* mesh,
		const Transform* meshTransform) const
	{
		return algo::FindGJKMaifoldPoints(
			this, transform, mesh, meshTransform);
	}
}
}
