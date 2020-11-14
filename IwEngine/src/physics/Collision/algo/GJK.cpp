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
		vector3 support = detail::Support(colliderA, transformA, colliderB, transformB, vector3::unit_x);

		// Simplex is an array of points, max count is 4
		Simplex points;
		points.push_front(support);

		// New direction is backwards from that point
		vector3 direction = -support;

		size_t iterations = 0;
		while (iterations++ < 32) {
			support = detail::Support(colliderA, transformA, colliderB, transformB, direction);

			if (support.dot(direction) <= 0) {
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
	//	std::vector<vector2> polytope(simplex.begin(), simplex.end());
	//
	//	vector2 minNormal;
	//	float   minDistance = FLT_MAX;
	//	size_t  minIndex = 0;
	//
	//	while (minDistance == FLT_MAX) {
	//		for (size_t i = 0; i < polytope.size(); i++) {
	//			vector2 a = polytope[i];
	//			vector2 b = polytope[(i + 1) % polytope.size()];
	//
	//			vector2 ab = b - a;
	//
	//			vector2 normal = vector2(ab.y, -ab.x).normalized();
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
	//		vector2 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);
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
		std::vector<vector3> polytope(simplex.begin(), simplex.end());
		std::vector<size_t>  faces = {
			0, 1, 2,
			0, 3, 1,
			0, 2, 3,
			1, 3, 2
		};
		
		auto [normals, minFace] = detail::GetFaceNormals(polytope, faces);

		vector3 minNormal;
		float   minDistance = FLT_MAX;
		
		size_t iterations = 0;
		while (minDistance == FLT_MAX) {
			minNormal   = normals[minFace].xyz();
			minDistance = normals[minFace].w;

			if (iterations++ > EPA_MAX_ITER) {
				break;
			}

			vector3 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);
			float sDistance = minNormal.dot(support);

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

		points.Normal = -minNormal;
		points.PenetrationDepth = minDistance + 0.001f;
		points.HasCollision = true;

		return points;
	}

namespace detail {
	vector3 Support(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB,
		vector3 direction)
	{
		return colliderA->FindFurthestPoint(transformA,  direction)
			 - colliderB->FindFurthestPoint(transformB, -direction);
	}

	std::pair<std::vector<vector4>, size_t> GetFaceNormals(
		const std::vector<vector3>& polytope,
		const std::vector<size_t>&  faces)
	{
		std::vector<vector4> normals;
		int minTriangle = -1;

		for (size_t i = 0; i < faces.size(); i += 3) {
			vector3 a = polytope[faces[i    ]];
			vector3 b = polytope[faces[i + 1]];
			vector3 c = polytope[faces[i + 2]];

			vector3 normal = (b - a).cross(c - a).normalized();
			float distance = normal.dot(a);

			if (distance < 0) {
				normal   *= -1;
				distance *= -1;
			}

			normals.emplace_back(normal, distance);

			if (   minTriangle == -1
				|| distance < normals[minTriangle].w)
			{
				minTriangle = i / 3;
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
		vector3& direction)
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
		vector3& direction)
	{
		vector3 a = points[0];
		vector3 b = points[1];

		vector3 ab = b - a;
		vector3 ao =   - a;

		if (SameDirection(ab, ao)) {
			direction = ab.cross(ao).cross(ab);
		}

		else {
			points = { a };
			direction = ao;
		}

		return false;
	}

	bool Triangle(
		Simplex& points,
		vector3& direction)
	{
		vector3 a = points[0];
		vector3 b = points[1];
		vector3 c = points[2];

		vector3 ab = b - a;
		vector3 ac = c - a;
		vector3 ao =   - a;

		vector3 abc = ab.cross(ac);

		if (SameDirection(abc.cross(ac), ao)) {
			if (SameDirection(ac, ao)) {
				points = { a, c };
				direction = ac.cross(ao).cross(ac);
			}

			else {
				return Line(points = { a, b }, direction);
			}
		}

		else {
			if (SameDirection(ab.cross(abc), ao)) {
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
		vector3& direction)
	{
		vector3 a = points[0];
		vector3 b = points[1];
		vector3 c = points[2];
		vector3 d = points[3];

		vector3 ab = b - a;
		vector3 ac = c - a;
		vector3 ad = d - a;
		vector3 ao =   - a;

		vector3 abc = ab.cross(ac);
		vector3 acd = ac.cross(ad);
		vector3 adb = ad.cross(ab);

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
		const vector3& direction,
		const vector3& ao)
	{
		return direction.dot(ao) > 0;
	}
}
}
}
}
