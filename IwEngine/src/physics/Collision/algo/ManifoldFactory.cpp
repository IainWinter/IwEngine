#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
namespace Physics {
namespace algo {
	Manifold MakeManifold(
		const Rigidbody* a,
		const Rigidbody* b)
	{
		detail::ManifoldPoints points = detail::FindManifoldPoints(a, b);

		if (points.NoContact) {
			return { 0, 0, 0, 0, 0, false };
		}

		return {
			a, b,
			points.A, points.B,
			(points.B - points.A).length(),
			true
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
				(SphereCollider*)a->Col(), a->Trans(),
				(SphereCollider*)b->Col(), b->Trans());
		}

		return { 0, 0, true };
	}

	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		iw::vector3 A = a->Center + ta->Position;
		iw::vector3 B = b->Center + tb->Position;

		iw::vector3 AtoB = B - A;
		iw::vector3 BtoA = A - B;

		if (   AtoB.length() > 2 * a->Radius /** ta->Scale*/
			|| BtoA.length() > 2 * b->Radius /** ta->Scale*/) // cant compare spheres that are scaled non uniformly should be v > rad * scale
		{
			return { 0, 0, true };
		}

		return {
			A + AtoB.normalized() * a->Radius,
			B + BtoA.normalized() * b->Radius,
			false
		};
	}
}
}
}
}
