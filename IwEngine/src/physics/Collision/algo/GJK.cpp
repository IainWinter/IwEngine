#include "iw/physics/Collision/algo/GJK.h"
#include <vector>
#include <array>

namespace iw {
namespace Physics {
namespace algo {
	bool GJK(
		const Collider* colliderA,
		const Collider* colliderB)
	{
		// Get initial support point in any direction
		vector3 support = detail::Support(colliderA, colliderB, vector3::unit_x);

		// Our simplex is an array of points, max count is 4
		std::array<vector3, 4> points{ support };

		// New direction is backwards from that point
		vector3 direction = -support;

		while (true) {
			support = detail::Support(colliderA, colliderB, direction);

			if (support.dot(direction) < 0) {
				return false; // no collision
			}

			points = { support, points[0], points[1], points[2] }; // add to front

			if (detail::NextSimplex(points, direction)) {
				return true;
			}
		}
	}

namespace detail {
	vector3 Support(
		const Collider* colliderA,
		const Collider* colliderB,
		const vector3& direction)
	{
		return colliderA->FindFurthestPoint( direction)
			 - colliderB->FindFurthestPoint(-direction);
	}

	bool NextSimplex(
		std::array<vector3, 4>& points, 
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
		std::array<vector3, 4>& points,
		vector3& direction)
	{
		const vector3& a = points[0];
		const vector3& b = points[1];

		vector3 ab = b - a;
		vector3 ao =   - a;

		if (SameDirection(ab, ao)) {
			points = { a, b };
			direction = ab.cross(ao).cross(ab);
		}

		else {
			points = { a };
			direction = ao;
		}

		return false;
	}

	bool Triangle(
		std::array<vector3, 4>& points,
		vector3& direction)
	{
		const vector3& a = points[0];
		const vector3& b = points[1];
		const vector3& c = points[2];

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
				return Line(points, direction);
			}
		}

		else if (SameDirection(ab.cross(abc), ao)) {
			return Line(points, direction);
		}

		else if (SameDirection(abc, ao)) {
			direction = abc;
		}

		else {
			direction = -abc;
		}

		return false;
	}

	bool Tetrahedron(
		std::array<vector3, 4>& points,
		vector3& direction)
	{
		vector3 a = points[0];
		vector3 b = points[1];
		vector3 c = points[2];
		vector3 d = points[3];

		vector3 abd = (b - a).cross(d - a);
		vector3 bcd = (c - b).cross(d - c);
		vector3 cad = (a - c).cross(d - a);

		if (SameDirection(abd, -a)) {
			return Triangle(points = { a, b, d }, direction);
		}

		if (SameDirection(bcd, -a)) {
			return Triangle(points = { b, c, d }, direction);
		}

		if (SameDirection(cad, -a)) {
			return Triangle(points = { c, a, d }, direction);
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
