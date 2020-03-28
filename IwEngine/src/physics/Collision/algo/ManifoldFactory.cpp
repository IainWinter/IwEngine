#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include <iw\log\logger.h>

namespace iw {
namespace Physics {
namespace algo {
	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		vector3 A = a->Center + ta->Position;
		vector3 B = b->Center + tb->Position;

		float Ar = a->Radius * ta->Scale.x;
		float Br = b->Radius * tb->Scale.x;

		vector3 AtoB = B - A;
		vector3 BtoA = A - B;

		if (AtoB.length() > Ar + Br) {
			return { 
				0, 0, 
				0, 
				0,
				false
			};
		}

		A += AtoB.normalized() * Ar;
		B += BtoA.normalized() * Br;

		AtoB = B - A;

		return {
			A, B,
			AtoB.normalized(),
			AtoB.length(),
			true
		};
	}

	// Transforms dont work for plane

	ManifoldPoints FindSpherePlaneMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb)
	{
		vector3 A = a->Center + ta->Position;
		vector3 N = b->Plane.P.normalized() * tb->Rotation + tb->Position;
		vector3 P = N * b->Plane.D;

		float Ar = a->Radius * ta->Scale.x;

		float d = (A - P).dot(N);

		if (d > Ar) {
			return {
				0, 0,
				0,
				0,
				false
			};
		}
		
		vector3 B = A - N * d;
		A = A - N * Ar;

		return {
			A, B,
			N.normalized(),
			(B - A).length(),
			true
		};
	}

	ManifoldPoints FindSphereCapsuleMaifoldPoints(
		const SphereCollider*  a, const Transform* ta,
		const CapsuleCollider* b, const Transform* tb)
	{
		vector3 A = a->Center   + ta->Position;
		vector3 B = b->Position + tb->Position;
		vector3 C = B + b->Offset;
		
		float Ar = a->Radius * ta->Scale.x;
		float Br = b->Radius * tb->Scale.x;

		vector3 BtoA = A - B;
		vector3 BtoC = C - B;

		float   d = iw::clamp(BtoC.normalized().dot(BtoA), 0.0f, BtoC.length());
		vector3 D = B + BtoC.normalized() * d;

		vector3 AtoD = D - A;
		vector3 DtoA = A - D;

		if (AtoD.length() > Ar + Br) {
			return {
				0, 0,
				0,
				0,
				false
			};
		}

		A += AtoD.normalized() * Ar;
		D += DtoA.normalized() * Br;

		AtoD = D - A;

		return {
			A, D,
			AtoD.normalized(),
			AtoD.length(),
			true
		};
	}
}
}
}
