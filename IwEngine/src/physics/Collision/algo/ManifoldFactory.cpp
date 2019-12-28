#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
namespace Physics {
namespace algo {
	Manifold MakeManifold(
		Rigidbody* a,
		Rigidbody* b)
	{
		detail::ManifoldPoints points = detail::FindManifoldPoints(a, b);

		if (points.NoContact) {
			return { 0, 0, 0, 0, 0, false };
		}

		return {
			a, b,
			points.A, points.B,
			points.Normal,
			(points.B - points.A).length(),
			true
		};
	}

namespace detail {
	ManifoldPoints FindManifoldPoints(
		CollisionObject* a,
		CollisionObject* b)
	{
		if (   a->Col()->Shape == ColliderShape::SPHERE
			&& b->Col()->Shape == ColliderShape::SPHERE)
		{
			return detail::FindSphereSphereMaifoldPoints(
				(SphereCollider*)a->Col(), a->Trans(),
				(SphereCollider*)b->Col(), b->Trans());
		}

		else if (a->Col()->Shape == ColliderShape::SPHERE
			  && b->Col()->Shape == ColliderShape::PLANE)
		{
			return detail::FindSpherePlaneMaifoldPoints(
				(SphereCollider*)a->Col(), a->Trans(),
				(PlaneCollider*)b->Col(), b->Trans());
		}

		return { 0, 0, 0, true };
	}

	ManifoldPoints FindSphereSphereMaifoldPoints(
		SphereCollider* a, Transform* ta,
		SphereCollider* b, Transform* tb)
	{
		iw::vector3 A = a->Center + ta->Position;
		iw::vector3 B = b->Center + tb->Position;

		iw::vector3 AtoB = B - A;
		iw::vector3 BtoA = A - B;

		if (   AtoB.length() > 2 * a->Radius /** ta->Scale*/
			|| BtoA.length() > 2 * b->Radius /** ta->Scale*/) // cant compare spheres that are scaled non uniformly should be v > rad * scale
		{
			return { 0, 0, 0, true };
		}

		return {
			A + AtoB.normalized() * a->Radius,
			B + BtoA.normalized() * b->Radius,
			AtoB.normalized(),
			false
		};
	}

	ManifoldPoints FindSpherePlaneMaifoldPoints(
		SphereCollider* a,
		Transform* ta,
		PlaneCollider* b,
		Transform* tb)
	{
		iw::vector3 A = a->Center + ta->Position;
		iw::vector3 N = b->Plane.P.normalized() * tb->Rotation + tb->Position;
		iw::vector3 P = N * b->Plane.D;

		float d = (A - P).dot(N);

		if (d > a->Radius) {
			return { 0, 0, 0, true };
		}
		
		return {
			A - N * a->Radius,
			A - N * d,
			-N.normalized(),
			false
		};
	}
}
}
}
}
