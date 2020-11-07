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

		while (true) {
			support = detail::Support(colliderA, transformA, colliderB, transformB, direction);

			if (support.dot(direction) <= 0) {
				return std::make_pair(false, points); // no collision
			}

			points.push_front(support);

			if (detail::NextSimplex(points, direction)) {
				return std::make_pair(true, points);
			}
		}
	}

	ManifoldPoints EPA(
		const Simplex& simplex,
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB)
	{
		std::vector<vector3> polytope(simplex.begin(), simplex.end());

		unsigned minI = 0;
		float   minDistance = FLT_MAX;
		vector3 minNormal;

		while (minDistance == FLT_MAX) {
			for (unsigned i = 0; i < polytope.size(); i++) {
				unsigned j = (i + 1u) % polytope.size();
				unsigned k = (i + 2u) % polytope.size();

				vector3 a = polytope[i];
				vector3 b = polytope[j];
				vector3 c = polytope[k];

				vector3 ab = b - a;
				vector3 ac = c - a;
				vector3 ao =   - a;

				vector3 abc = ab.cross(ac);

				vector3 normal = abc.normalized();
				float distance = normal.dot(ao);

				if (distance < 0) {
					distance *= -1;
					normal   *= -1; // normal points away from origin, this is caused by inconsistant winding order
				}

				if (distance < minDistance) {
					minDistance = distance;
					minNormal   = normal;
					minI = i;
				}
			}

			vector3 support = detail::Support(colliderA, transformA, colliderB, transformB, minNormal);

			float d1 = support.dot(minNormal);
			float d2 = polytope[minI].dot(minNormal); // problem in here
			float difference = abs(d1 - d2);

			if (difference > 0.0001f) {
				polytope.insert(polytope.begin() + minI, support); // add another point inbetween i and j
				minDistance = FLT_MAX; // reset while loop
			}
		}

		ManifoldPoints points;
		points.A = 0;//?polytope[minI];
		points.B = 0;//?polytope[minJ];

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
