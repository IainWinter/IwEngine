#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
	template<>
	ManifoldPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const Physics::SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		return algo::FindPlaneSphereMaifoldPoints(
			this, transform, sphere, sphereTransform);
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
