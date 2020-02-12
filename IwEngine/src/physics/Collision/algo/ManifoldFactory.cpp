#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace iw {
namespace Physics {
namespace algo {
	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		iw::vector3 A = a->Center + ta->Position;
		iw::vector3 B = b->Center + tb->Position;

		iw::vector3 AtoB = B - A;
		iw::vector3 BtoA = A - B;

		if (AtoB.length() - b->Radius > a->Radius) {
			return { 
				0, 0, 
				0, 
				0,
				false
			};
		}

		A += AtoB.normalized() * a->Radius;
		B += BtoA.normalized() * b->Radius;

		return {
			A, B,
			BtoA.normalized(),
			(B - A).length(),
			true
		};
	}

	ManifoldPoints FindSpherePlaneMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb)
	{
		iw::vector3 A = a->Center + ta->Position;
		iw::vector3 N = b->Plane.P.normalized() * tb->Rotation + tb->Position;
		iw::vector3 P = N * b->Plane.D;

		float d = (A - P).dot(N);
		if (d > a->Radius) {
			return {
				0, 0,
				0,
				0,
				false
			};
		}
		
		iw::vector3 B = A - N * d;
		A = A - N * a->Radius;

		return {
			A, B,
			N.normalized(),
			(B - A).length(),
			true
		};
	}

	ManifoldPoints FindPlaneSphereMaifoldPoints(
		const PlaneCollider*  a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		ManifoldPoints points = FindSpherePlaneMaifoldPoints(b, tb, a, ta);

		iw::vector3 v = points.A;
		points.A = points.B;
		points.B = v;
		points.Normal = -points.Normal;

		return points;
	}
}
}
}
