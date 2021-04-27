#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/physics/Collision/algo/GJK.h"
#include <iw\log\logger.h>

#include "glm/common.hpp"

namespace iw {
namespace Physics {
namespace algo {
	ManifoldPoints TestCollision(
		const SphereCollider* a, const Transform* ta,
		const SphereCollider* b, const Transform* tb)
	{
		glm::vec3 A = a->Center + ta->WorldPosition();
		glm::vec3 B = b->Center + tb->WorldPosition();

		float Ar = a->Radius * major(ta->WorldScale());
		float Br = b->Radius * major(tb->WorldScale());

		glm::vec3 AtoB = B - A;
		glm::vec3 BtoA = A - B;

		if (AtoB.length() > Ar + Br) {
			return {};
		}

		A += glm::normalize(AtoB) * Ar;
		B += glm::normalize(BtoA) * Br;

		BtoA = A - B;

		return {
			A, B,
			glm::normalize(BtoA),
			BtoA.length(),
			true
		};
	}

	// Transforms dont work for plane

	ManifoldPoints TestCollision(
		const SphereCollider* a, const Transform* ta,
		const PlaneCollider*  b, const Transform* tb)
	{
		glm::vec3 A  = a->Center + ta->WorldPosition();
		float     Ar = a->Radius * major(ta->WorldScale());

		glm::vec3 N = glm::normalize(b->Plane.P * tb->WorldRotation());
		glm::vec3 P = N * b->Plane.D + tb->WorldPosition();

		float d = glm::dot(A - P, N); // distance from center of sphere to plane surface

		if (d > Ar) {
			return {};
		}
		
		glm::vec3 B = A - N * d;
		        A = A - N * Ar;

		glm::vec3 BtoA = A - B;

		return {
			A, B,
			glm::normalize(BtoA),
			BtoA.length(),
			true
		};
	}

	ManifoldPoints TestCollision(
		const SphereCollider*  a, const Transform* ta,
		const CapsuleCollider* b, const Transform* tb)
	{
		float Bhs = 1.0f;
		float Brs = 1.0f;

		glm::vec3 s = tb->WorldScale();
		if (b->Direction == glm::vec3(1, 0, 0)) {
			Bhs = s.x;
			Brs = major(glm::vec2(s.y, s.z));
		}

		else if (b->Direction == glm::vec3(0, 1, 0)) {
			Bhs = s.y;
			Brs = major(glm::vec2(s.x, s.z));
		}

		else if (b->Direction == glm::vec3(0, 0, 1)) {
			Bhs = s.z;
			Brs = major(glm::vec2(s.x, s.y));
		}

		glm::vec3 offset = b->Direction * tb->WorldRotation() * (b->Height * Bhs / 2 - b->Radius * Brs);

		glm::vec3 A = a->Center          + ta->WorldPosition();
		glm::vec3 B = b->Center - offset + tb->WorldPosition();
		glm::vec3 C = b->Center + offset + tb->WorldPosition(); // might not be correct
		
		float Ar = a->Radius * major(ta->WorldScale());
		float Br = b->Radius * Brs;

		glm::vec3 BtoA = A - B;
		glm::vec3 BtoC = C - B;

		float   d = glm::clamp<float>(glm::dot(glm::normalize(BtoC), BtoA), 0.0f, BtoC.length());
		glm::vec3 D = B + glm::normalize(BtoC) * d;

		glm::vec3 AtoD = D - A;
		glm::vec3 DtoA = A - D;

		if (AtoD.length() > Ar + Br) {
			return {};
		}



		A += glm::normalize(AtoD) * Ar;
		D += glm::normalize(DtoA) * Br;

		DtoA = A - D;

		return {
			A, D,
			glm::normalize(DtoA),
			DtoA.length(),
			true
		};
	}

	ManifoldPoints TestCollision(
		const PlaneCollider* a, const Transform* ta,
		const MeshCollider*  b, const Transform* tb)
	{
		glm::vec3 N = a->Plane.P * ta->WorldRotation();
		glm::normalize(N);

		glm::vec3 P = N * a->Plane.D + ta->WorldPosition();

		glm::vec3 B = b->FindFurthestPoint(tb, -N);

		glm::vec3 BtoP = P - B;
		
		float distance = glm::dot(BtoP, N);

		if (distance < 0) {
			return {};
		}

		return {
			B, P,
			N,
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

		return {glm::vec3(), glm::vec3(), glm::vec3(), 0, result.first};
	}

	// Swaps

	void SwapPoints(
		ManifoldPoints& points)
	{
		glm::vec3 T = points.A;
		points.A = points.B;
		points.B = T;

		points.Normal = -points.Normal;
	}

	ManifoldPoints TestCollision(
		const PlaneCollider*  a, const Transform* ta, 
		const SphereCollider* b, const Transform* tb)
	{
		ManifoldPoints points = TestCollision(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}

	ManifoldPoints TestCollision(
		const MeshCollider*  a, const Transform* ta,
		const PlaneCollider* b, const Transform* tb)
	{
		ManifoldPoints points = TestCollision(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}

	ManifoldPoints TestCollision(
		const CapsuleCollider* a, const Transform* ta,
		const SphereCollider*  b, const Transform* tb)
	{
		ManifoldPoints points = TestCollision(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}
}
}
}
