#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/physics/Collision/algo/GJK.h"
#include <iw\log\logger.h>

#include "glm/common.hpp"

namespace iw {
namespace Physics {
namespace algo {

	// helpers

	ManifoldPoints SwapPoints(
		ManifoldPoints& points)
	{
		glm::vec3 T = points.A;
		points.A = points.B;
		points.B = T;

		points.Normal = -points.Normal;

		return points;
	}

	// funcs

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
		const HullCollider*  b, const Transform* tb)
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

	// this is how all the functions should be refactored
	ManifoldPoints TestCollision_Mesh_notMesh(
		const Collider* mesh,  const Transform* tmesh,
		const Collider* other, const Transform* tother)
	{
		assert(mesh ->Type() == ColliderType::MESH
			&& other->Type() != ColliderType::MESH);
		
		std::vector<ManifoldPoints> manifolds;

		for (HullCollider& part : ((MeshCollider*)mesh)->GetHullParts())
		{
			if (!part.Bounds().Intersects(tmesh, other->Bounds(), tother)) continue;

			auto [hasCollision, simplex] = GJK(&part, tmesh, other, tother);
			if (hasCollision) {
				manifolds.push_back(EPA(simplex, &part, tmesh, other, tother));
			}
		}

		if (manifolds.size() == 0) return {}; // exit if no collision

		size_t maxNormalIndex = 0;
		float  maxNormalDist  = FLT_MIN;

		for (size_t i = 0; i < manifolds.size(); i++) {
			if (manifolds[i].PenetrationDepth > maxNormalDist) {
				maxNormalDist = manifolds[i].PenetrationDepth;
				maxNormalIndex = i;
			}
		}

		return manifolds[maxNormalIndex];
	}

	ManifoldPoints FindGJKMaifoldPoints(
		const Collider* a, const Transform* ta,
		const Collider* b, const Transform* tb) // could have bool for if we are only checking triggers, could save compute
	{
		bool aMesh = a->Type() == ColliderType::MESH;
		bool bMesh = b->Type() == ColliderType::MESH;

		if (aMesh && bMesh) return {}; // Don't collide mesh v mesh

		auto result = GJK(a, ta, b, tb); // has collision, simplex

		if (result.first)
		{
			if (aMesh) {
				return TestCollision_Mesh_notMesh(a, ta, b, tb);
			}

			else
			if (bMesh) {
				ManifoldPoints p = TestCollision_Mesh_notMesh(b, tb, a, ta);
				return SwapPoints(p);
			}
				
			else {
				return EPA(result.second, a, ta, b, tb);
			}

			//int partUsed = -1;

			//auto testAllParts = [&](
			//	const Collider* a, const Transform* ta,
			//	const Collider* b, const Transform* tb)
			//{
			//	result.first = false;

			//	MeshCollider* mesh = (MeshCollider*)a;

			//	for (HullCollider& part : mesh->GetHullParts()) {
			//		result = GJK(&part, ta, b, tb);
			//		if (result.first) {

			//			break;
			//		}
			//	}
			//};
			//
			//if (   a->Type() == ColliderType::MESH
			//	&& b->Type() != ColliderType::MESH)
			//{
			//	testAllParts(a, ta, b, tb);
			//}

			//else
			//if (   a->Type() != ColliderType::MESH
			//	&& b->Type() == ColliderType::MESH)
			//{
			//	testAllParts(b, tb, a, ta);
			//}

			//else
			//if (   a->Type() == ColliderType::MESH
			//	&& b->Type() == ColliderType::MESH)
			//{
			//	goto exit; // ews, todo: refactor all the TestCollision functions 
			//}

			//if (result.first) {
			//	return EPA(result.second, a, ta, b, tb);
			//}
		}

		//exit:
		return {};
	}

	// Swaps

	ManifoldPoints TestCollision(
		const PlaneCollider*  a, const Transform* ta, 
		const SphereCollider* b, const Transform* tb)
	{
		ManifoldPoints points = TestCollision(b, tb, a, ta);
		SwapPoints(points);

		return points;
	}

	ManifoldPoints TestCollision(
		const HullCollider*  a, const Transform* ta,
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
