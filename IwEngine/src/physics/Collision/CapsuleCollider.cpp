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
		ManifoldPoints points = sphere->TestCollision(sphereTransform, this, transform);
		iw::vector3 T = points.A;
		points.A = points.B;
		points.B = T;

		points.Normal = -points.Normal;

		return points;
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
