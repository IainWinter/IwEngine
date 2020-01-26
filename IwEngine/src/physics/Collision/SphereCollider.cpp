#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
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
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{
		return algo::FindSpherePlaneMaifoldPoints(
			this, transform, plane, planeTransform);
	}
}
