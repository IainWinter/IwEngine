#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/physics/Collision/algo/GJK.h"
#include <iw\log\logger.h>

namespace iw {
namespace Physics {
namespace algo {
	ManifoldPoints FindSphereSphereMaifoldPoints(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		vector3 A = a->Center + ta->WorldPosition();
		vector3 B = b->Center + tb->WorldPosition();

		float Ar = a->Radius * ta->WorldScale().major();
		float Br = b->Radius * tb->WorldScale().major();

		vector3 AtoB = B - A;
		vector3 BtoA = A - B;

		if (AtoB.length() > Ar + Br) {
			return {};
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
		vector3 A  = a->Center + ta->WorldPosition();
		float   Ar = a->Radius * ta->WorldScale().major();

		vector3 N = b->Plane.P * tb->WorldRotation();
		N.normalize();
		
		vector3 P = N * b->Plane.D + tb->WorldPosition();

		float d = (A - P).dot(N); // distance from center of sphere to plane surface

		if (d > Ar) {
			return {};
		}
		
		vector3 B = A - N * d;
		        A = A - N * Ar;

		vector3 AtoB = B - A;

		return {
			A, B,
			AtoB.normalized(),
			AtoB.length(),
			true
		};
	}

	ManifoldPoints FindSphereCapsuleMaifoldPoints(
		const SphereCollider*  a, const Transform* ta,
		const CapsuleCollider* b, const Transform* tb)
	{
		float Bhs = 1.0f;
		float Brs = 1.0f;

		vector3 s = tb->WorldScale();
		if (b->Direction == vector3::unit_x) {
			Bhs = s.x;
			Brs = vector2(s.y, s.z).major();
		}

		else if (b->Direction == vector3::unit_y) {
			Bhs = s.y;
			Brs = vector2(s.x, s.z).major();
		}

		else if (b->Direction == vector3::unit_z) {
			Bhs = s.z;
			Brs = vector2(s.x, s.y).major();
		}

		vector3 offset = b->Direction * tb->WorldRotation() * (b->Height * Bhs / 2 - b->Radius * Brs);

		vector3 A = a->Center          + ta->WorldPosition();
		vector3 B = b->Center - offset + tb->WorldPosition();
		vector3 C = b->Center + offset + tb->WorldPosition(); // might not be correct
		
		float Ar = a->Radius * ta->WorldScale().major();
		float Br = b->Radius * Brs;

		vector3 BtoA = A - B;
		vector3 BtoC = C - B;

		float   d = iw::clamp(BtoC.normalized().dot(BtoA), 0.0f, BtoC.length());
		vector3 D = B + BtoC.normalized() * d;

		vector3 AtoD = D - A;
		vector3 DtoA = A - D;

		if (AtoD.length() > Ar + Br) {
			return {};
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

	ManifoldPoints FindPlaneMeshMaifoldPoints(
		const PlaneCollider* a, const Transform* ta,
		const MeshCollider*  b, const Transform* tb)
	{
		vector3 N = a->Plane.P * ta->WorldRotation();
		N.normalize();

		vector3 P = N * a->Plane.D + ta->WorldPosition();

		vector3 B = b->FindFurthestPoint(tb, -N);

		vector3 BtoP = P - B;
		
		float distance = BtoP.dot(N);

		if (distance < 0) {
			return {};
		}

		return {
			B, P,
			-N,
			distance,
			true
		};
	}

	ManifoldPoints FindGJKMaifoldPoints(
		const Collider* a, const Transform* ta,
		const Collider* b, const Transform* tb) // could have bool for if we are only checking triggers, could save compute
	{
		auto result = GJK(a, ta, b, tb);

		if (result.first) {
			return EPA(result.second, a, ta, b, tb);
		}

		return {0, 0, 0, 0, result.first};
	}

	// Swaps

	void SwapPoints(
		ManifoldPoints& points)
	{
		iw::vector3 T = points.A;
		points.A = points.B;
		points.B = T;

		points.Normal = -points.Normal;
	}

	ManifoldPoints FindPlaneSphereMaifoldPoints(
		const PlaneCollider*  a, const Transform* ta, 
		const SphereCollider* b, const Transform* tb)
	{
		ManifoldPoints points = FindSpherePlaneMaifoldPoints(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}

	ManifoldPoints FindMeshPlaneMaifoldPoints(
		const MeshCollider*  a, const Transform* ta,
		const PlaneCollider* b, const Transform* tb)
	{
		ManifoldPoints points = FindPlaneMeshMaifoldPoints(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}

	ManifoldPoints FindCapsuleSphereMaifoldPoints(
		const CapsuleCollider* a, const Transform* ta,
		const SphereCollider*  b, const Transform* tb)
	{
		ManifoldPoints points = FindSphereCapsuleMaifoldPoints(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}
}
}
}
