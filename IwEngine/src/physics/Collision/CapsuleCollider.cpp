#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
	template<>
	ManifoldPoints CapsuleCollider::TestCollision(
		const Transform* transform,
		const Physics::SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		return sphere->TestCollision(sphereTransform, this, transform);
	}

	template<>
	ManifoldPoints CapsuleCollider::TestCollision(
		const Transform* transform, 
		const CapsuleCollider* capsule, 
		const Transform* capsuleTransform) const
	{
		return {};//algo::FindCapsuleCapsuleMaifoldPoints(
			//this, transform, plane, planeTransform);
	}

	template<>
	ManifoldPoints CapsuleCollider::TestCollision(
		const Transform* transform,
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return {};
	}
}
}
