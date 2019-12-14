#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
namespace Physics {
namespace algo {
	Manifold MakeManifold(
		const Rigidbody* a,
		const Rigidbody* b)
	{
		detail::ManifoldPoints points = detail::FindManifoldPoints(a, b);

		return {
			a, b,
			points.A, points.B,
			(points.B - points.A).length()
		};
	}

namespace detail {
	ManifoldPoints FindManifoldPoints(
		const CollisionObject* a,
		const CollisionObject* b)
	{
		if (   a->Col()->Shape == ColliderShape::SPHERE
			&& b->Col()->Shape == ColliderShape::SPHERE)
		{
			return detail::FindSphereSphereMaifoldPoints(
				(SphereCollider*)a->Col(),
				(SphereCollider*)b->Col());
		}

		return { 0, 0 };
	}

	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a,
		const SphereCollider* b)
	{
		iw::vector3 BtoA = a->Center - b->Center;
		BtoA.normalize();

		iw::vector3 AtoB = b->Center - a->Center;
		AtoB.normalize();

		return {
			AtoB * a->Radius,
			BtoA * b->Radius
		};
	}
}
}
}
}
