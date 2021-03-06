#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
	template<>
	vector3 SphereCollider::FindFurthestPoint(
		const Transform* transform,
		vector3 direction) const
	{
		//vector3 xz = vector3(direction.x, 0, direction.z).normalized() * Radius;   // todo: make cylender collider
		//xz.y = (direction.y < 0) ? -Radius : Radius; // low : high

		//return xz * transform->WorldScale().major() + Center + transform->WorldPosition();

		return Center + transform->WorldPosition()
			 + Radius * direction.normalized() * transform->WorldScale().major();
	}

	template<>
	ManifoldPoints SphereCollider::TestCollision(
		const Transform* transform,
		const SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		return algo::FindSphereSphereMaifoldPoints(
			this, transform, sphere, sphereTransform);
	}

	template<>
	ManifoldPoints SphereCollider::TestCollision(
		const Transform* transform,
		const Physics::CapsuleCollider* capsule,
		const Transform* capsuleTransform) const
	{
		return algo::FindSphereCapsuleMaifoldPoints(
			this, transform, capsule, capsuleTransform);
	}

	template<>
	ManifoldPoints SphereCollider::TestCollision(
		const Transform* transform,
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return algo::FindSpherePlaneMaifoldPoints(
			this, transform, plane, planeTransform);
	}

	template<>
	ManifoldPoints SphereCollider::TestCollision(
		const Transform* transform,
		const Physics::MeshCollider* mesh,
		const Transform* meshTransform) const
	{
		return algo::FindGJKMaifoldPoints(
			this, transform, mesh, meshTransform);
	}
}
}
