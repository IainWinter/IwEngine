#include "iw/physics/Collision/CylinderCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
	template<>
	ManifoldPoints CylinderCollider::TestCollision(
		const Transform* transform,
		const Physics::SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		return {};
	}

	template<>
	ManifoldPoints CylinderCollider::TestCollision(
		const Transform* transform, 
		const Physics::CapsuleCollider* capsule,
		const Transform* capsuleTransform) const
	{
		return {};
	}

	template<>
	ManifoldPoints CylinderCollider::TestCollision(
		const Transform* transform,
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return {};
	}
	
	template<>
	ManifoldPoints CylinderCollider::TestCollision(
		const Transform* transform, 
		const Physics::MeshCollider* mesh,
		const Transform* meshTransform) const
	{
		return {};
	}
}
}
