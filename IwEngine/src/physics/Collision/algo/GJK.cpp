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
	//	const Collider2* colliderA, const Transform* transformA,
	//	const Collider2* colliderB, const Transform* transformB)
	//{
	//	std::vector<vector2> polytope(simplex.begin(), simplex.end());

	//	vector2 minNormal;
	//	float   minDistance = FLT_MAX;
	//	size_t  minIndex = 0;

	//	while (minDistance == FLT_MAX) {
	//		for (size_t i = 0; i < polytope.size(); i++) {
	//			vector2 a = polytope[i];
	//			vector2 b = polytope[(i + 1) % polytope.size()];

	//			vector2 ab = b - a;

	//			vector2 normal = vector2(ab.y, -ab.x).normalized();
	//			float distance = normal.dot(-a);

	//			if (distance < 0) {
	//				normal   *= -1;
	//				distance *= -1;
	//			}

	//			if (distance < minDistance) {
	//				minNormal   = normal;
	//				minDistance = distance;
	//				minIndex    = i;
	//			}
	//		}

	//		vector3 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);

	//		if (std::find(polytope.begin(), polytope.end(), support) == polytope.end()) {
	//			minDistance = FLT_MAX;
	//			polytope.push_back(support);
	//		}
	//	}

	//	vector3 o = polytope[index[minTriangle]];

	//	ManifoldPoints points;
	//	points.A = o - minNormal * minDistance;
	//	points.B = o + minNormal * minDistance;

	//	points.Normal = minNormal;
	//	points.PenetrationDepth = minDistance;
	//	points.HasCollision = true;

	//	return points;
	//}

	ManifoldPoints EPA(
		const Simplex& simplex,
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB)
	{
		std::vector<vector3> polytope(simplex.begin(), simplex.end());
		std::vector<size_t>  index = {
			0, 2, 1,
			0, 1, 3,
			0, 3, 2,
			1, 2, 3,
		};

		vector3 minNormal;
		float   minDistance = FLT_MAX;
		size_t  minTriangle = 0;

		while (minDistance == FLT_MAX) {
			for (size_t i = 0; i < index.size(); i += 3) {
				vector3 a = polytope[index[i    ]];
				vector3 b = polytope[index[i + 1]];
				vector3 c = polytope[index[i + 2]];

				vector3 normal = (b - a).cross(c - a).normalized();
				float distance = normal.dot(-a);

				if (distance < 0) {
					normal   *= -1;
					distance *= -1;
				}

				if (distance < minDistance) {
					minNormal   = normal;
					minDistance = distance;
					minTriangle = i;
				}
			}

			vector3 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);

			if (std::find(polytope.begin(), polytope.end(), support) == polytope.end()) {
				minDistance = FLT_MAX;

				size_t a = index[minTriangle    ];
				size_t b = index[minTriangle + 1];
				size_t c = index[minTriangle + 2];
				size_t d = polytope.size();

				index.pop_back();   index.pop_back();   index.pop_back();

				index.push_back(a); index.push_back(c); index.push_back(d);
				index.push_back(b); index.push_back(d); index.push_back(c);
				index.push_back(a); index.push_back(d); index.push_back(b);

				polytope.push_back(support);
			}
		}

		vector3 o = polytope[index[minTriangle]];

		ManifoldPoints points;
		points.A = o + minNormal * minDistance;
		points.B = o - minNormal * minDistance;

		points.Normal = minNormal;
		points.PenetrationDepth = minDistance;
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
