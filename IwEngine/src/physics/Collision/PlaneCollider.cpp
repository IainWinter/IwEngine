#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
	template<>
	ManifoldPoints PlaneCollider::TestCollision(
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
	ManifoldPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const Physics::CapsuleCollider* capsule,
		const Transform* capsuleTransform) const
	{
		return {};
	}

	template<>
	ManifoldPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return {}; /*algo::FindPlanePlaneMaifoldPoints(
			this, transform, plane, planeTransform);*/
	}
}
}
