#include "iw/physics/Collision/algo/GJK.h"
#include <vector>

namespace iw {
namespace Physics {
namespace algo {
	std::pair<bool, Simplex> GJK(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB)
	{
		// Get initial support point in any direction
		glm::vec3 support = detail::Support(colliderA, transformA, colliderB, transformB, glm::vec3(1, 0, 0));

		// Simplex is an array of points, max count is 4
		Simplex points;
		points.push_front(support);

		// New direction is backwards from that point
		glm::vec3 direction = -support;

		size_t iterations = 0;
		while (iterations++ < 32) {
			support = detail::Support(colliderA, transformA, colliderB, transformB, direction);

			if (glm::dot(support, direction) <= 0) {
				break;
			}

			points.push_front(support);

			if (detail::NextSimplex(points, direction)) {
				return std::make_pair(true, points);
			}
		}

		return std::make_pair(false, points); // no collision
	}

	//ManifoldPoints EPA(
	//	const Simplex& simplex,
	//	const Collider* colliderA, const Transform* transformA,
	//	const Collider* colliderB, const Transform* transformB)
	//{
	//	std::vector<glm::vec2> polytope(simplex.begin(), simplex.end());
	//
	//	glm::vec2 minNormal;
	//	float   minDistance = FLT_MAX;
	//	size_t  minIndex = 0;
	//
	//	while (minDistance == FLT_MAX) {
	//		for (size_t i = 0; i < polytope.size(); i++) {
	//			glm::vec2 a = polytope[i];
	//			glm::vec2 b = polytope[(i + 1) % polytope.size()];
	//
	//			glm::vec2 ab = b - a;
	//
	//			glm::vec2 normal = glm::vec2(ab.y, -ab.x).normalized();
	//			float distance = normal.dot(-a);
	//
	//			if (distance < 0) {
	//				normal   *= -1;
	//				distance *= -1;
	//			}
	//
	//			if (distance < minDistance) {
	//				minNormal   = normal;
	//				minDistance = distance;
	//				minIndex    = i;
	//			}
	//		}
	//
	//		glm::vec2 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);
	//		float sDistance = minNormal.dot(support);
	//
	//		if (fabsf(sDistance - minDistance) > 0.001f) {
	//			minDistance = FLT_MAX;
	//			polytope.insert(polytope.begin() + minIndex, support);
	//		}
	//	}
	//
	//	ManifoldPoints points;
	//	points.Normal = minNormal;
	//	points.PenetrationDepth = minDistance;
	//	points.HasCollision = true;
	//
	//	return points;
	//}

#ifndef EPA_MAX_ITER
#	define EPA_MAX_ITER 32
#endif

	ManifoldPoints EPA(
		const Simplex& simplex,
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB)
	{
		std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
		std::vector<size_t>  faces = {
			0, 1, 2,
			0, 3, 1,
			0, 2, 3,
			1, 3, 2
		};
		
		auto [normals, minFace] = detail::GetFaceNormals(polytope, faces);

		glm::vec3 minNormal;
		float   minDistance = FLT_MAX;
		
		size_t iterations = 0;
		while (minDistance == FLT_MAX) {
			minNormal   = glm::vec3(normals[minFace]);
			minDistance = normals[minFace].w;

			if (iterations++ > EPA_MAX_ITER) {
				break;
			}

			glm::vec3 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);
			float sDistance = glm::dot(minNormal, support);

			if (abs(sDistance - minDistance) > 0.001f) {
				minDistance = FLT_MAX;

				std::vector<std::pair<size_t, size_t>> uniqueEdges;

				for (size_t i = 0; i < normals.size(); i++) {
					if (detail::SameDirection(normals[i], support)) {
						size_t f = i * 3;

						detail::AddIfUniqueEdge(uniqueEdges, faces, f,     f + 1);
						detail::AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
						detail::AddIfUniqueEdge(uniqueEdges, faces, f + 2, f    );

						faces[f + 2] = faces.back(); faces.pop_back();
						faces[f + 1] = faces.back(); faces.pop_back();
						faces[f    ] = faces.back(); faces.pop_back();

						normals[i] = normals.back(); normals.pop_back();

						i--;
					}
				}

				if (uniqueEdges.size() == 0) {
					break;
				}

				std::vector<size_t> newFaces;
				for (auto [edge1, edge2] : uniqueEdges) {
					newFaces.push_back(edge1);
					newFaces.push_back(edge2);
					newFaces.push_back(polytope.size());
				}

				polytope.push_back(support);

				auto [newNormals, newMinFace] = detail::GetFaceNormals(polytope, newFaces);

				float newMinDistance = FLT_MAX;
				for (size_t i = 0; i < normals.size(); i++) {
					if (normals[i].w < newMinDistance) {
						newMinDistance = normals[i].w;
						minFace = i;
					}
				}

				if (newNormals[newMinFace].w < newMinDistance) {
					minFace = newMinFace + normals.size();
				}

				faces  .insert(faces  .end(), newFaces  .begin(), newFaces  .end());
				normals.insert(normals.end(), newNormals.begin(), newNormals.end());
			}
		}

		if (minDistance == FLT_MAX) {
			return {};
		}

		ManifoldPoints points;

		points.Normal = minNormal;
		points.PenetrationDepth = minDistance + 0.001f;
		points.HasCollision = true;

		return points;
	}

namespace detail {
	glm::vec3 Support(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB,
		glm::vec3 direction)
	{
		return colliderA->FindFurthestPoint(transformA,  direction)
			 - colliderB->FindFurthestPoint(transformB, -direction);
	}

	std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
		const std::vector<glm::vec3>& polytope,
		const std::vector<size_t>&  faces)
	{
		std::vector<glm::vec4> normals;
		size_t minTriangle = 0;
		float  minDistance = FLT_MAX;

		for (size_t i = 0; i < faces.size(); i += 3) {
			glm::vec3 a = polytope[faces[i    ]];
			glm::vec3 b = polytope[faces[i + 1]];
			glm::vec3 c = polytope[faces[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
			float distance = glm::dot(normal, a);

			if (distance < 0) {
				normal   *= -1;
				distance *= -1;
			}

			normals.emplace_back(normal, distance);

			if (distance < minDistance) {
				minTriangle = i / 3;
				minDistance = distance;
			}
		}

		return { normals, minTriangle };
	}

	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b)
	{
		auto reverse = std::find(              //      0--<--3
			edges.begin(),                     //     / \ B /   A: 2-0
			edges.end(),                       //    / A \ /    B: 0-2
			std::make_pair(faces[b], faces[a]) //   1-->--2
		);

		if (reverse != edges.end()) {
			edges.erase(reverse);
		}

		else {
			edges.emplace_back(faces[a], faces[b]);
		}
	}

	bool NextSimplex(
		Simplex& points,
		glm::vec3& direction)
	{
		switch (points.size()) {
			case 2: return Line       (points, direction);
			case 3: return Triangle   (points, direction);
			case 4: return Tetrahedron(points, direction);
		}
		
		// never should be here
		return false;
	}

	bool Line(
		Simplex& points,
		glm::vec3& direction)
	{
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];

		glm::vec3 ab = b - a;
		glm::vec3 ao =   - a;

		if (SameDirection(ab, ao)) {
			direction = glm::cross(glm::cross(ab, ao), ab);
		}

		else {
			points = { a };
			direction = ao;
		}

		return false;
	}

	bool Triangle(
		Simplex& points,
		glm::vec3& direction)
	{
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];
		glm::vec3 c = points[2];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ao =   - a;

		glm::vec3 abc = glm::cross(ab, ac);

		if (SameDirection(glm::cross(abc, ac), ao)) {
			if (SameDirection(ac, ao)) {
				points = { a, c };
				direction = glm::cross(glm::cross(ac, ao), ac);
			}

			else {
				return Line(points = { a, b }, direction);
			}
		}

		else {
			if (SameDirection(glm::cross(ab, abc), ao)) {
				return Line(points = { a, b }, direction);
			}

			else {
				if (SameDirection(abc, ao)) {
					direction = abc;
				}

				else {
					points = { a, c, b };
					direction = -abc;
				}
			}
		}

		return false;
	}

	bool Tetrahedron(
		Simplex& points,
		glm::vec3& direction)
	{
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];
		glm::vec3 c = points[2];
		glm::vec3 d = points[3];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ad = d - a;
		glm::vec3 ao =   - a;

		glm::vec3 abc = glm::cross(ab, ac);
		glm::vec3 acd = glm::cross(ac, ad);
		glm::vec3 adb = glm::cross(ad, ab);

		if (SameDirection(abc, ao)) {
			return Triangle(points = { a, b, c }, direction);
		}
		
		if (SameDirection(acd, ao)) {
			return Triangle(points = { a, c, d }, direction);
		}

		if (SameDirection(adb, ao)) {
			return Triangle(points = { a, d, b }, direction);
		}

		return true;
	}

	bool SameDirection(
		const glm::vec3& direction,
		const glm::vec3& ao)
	{
		return glm::dot(direction, ao) > 0;
	}
}
}
}
}
