#include "iw/physics/Collision/Algorithm/GJK.h"
#include <vector>

namespace IwPhysics {
	bool GJK(
		const Collider& collider1,
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2)
	{
		iwmath::vector3 points[4];
		iwmath::vector3 direction  = iwmath::vector3::unit_x;
		iwmath::vector3 supportVec = Support(collider1, collider2, 
			transformation1, transformation2, direction);

		points[0] = supportVec;
		direction = -supportVec;

		bool colliding = false;
		int pointCount = 1;

		while (!colliding) {
			iwmath::vector3 a = Support(collider1, collider2, 
				transformation1, transformation2, direction);

			if (a.dot(direction) <= 0) {
				break;
			}

			pointCount++;
			points[pointCount - 1] = a;

			//Simplex checks
			switch (pointCount) {
				case 2: direction = Simplex(a, points[0]); break;
				case 3: direction = Simplex(a, points[1], points[0]); break;
				case 4: {
					direction = Simplex(a, points[2], points[1], points[0]);
					if (direction == 0) {
						colliding = true;
					}

					else {
						points[0] = points[1];
						points[1] = points[2];
						points[2] = points[3];
					}

					break;
				}
			}
		}

		return colliding;// { collider1, collider2, /*poi*/iwmath::vector3() };
	}

	bool SameDirection(
		const iwm::vector3& direction, 
		const iwm::vector3& dirToOrigin) 
	{
		return direction.dot(dirToOrigin) > 0;
	}

	iwmath::vector3 Support(
		const Collider& bounds1,
		const Collider& bounds2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2,
		const iwmath::vector3& direction)
	{
		return bounds1.Support(transformation1)
			 - bounds2.Support(transformation2);
	}

	// Line
	iwmath::vector3 Simplex(
		const iwmath::vector3& a,
		const iwmath::vector3& b)
	{
		if (SameDirection(b - a, -a)) {
			return (b - a).cross(-a).cross(b - a);
		}

		else {
			return -a;
		}
	}

	// Triangle
	iwmath::vector3 Simplex(
		const iwmath::vector3 & a,
		const iwmath::vector3 & b,
		const iwmath::vector3 & c)
	{
		iwmath::vector3 abc = (b - a).cross(c - a);
		if (SameDirection(abc.cross(c - a), -a)) {
			if (SameDirection(c - a, -a)) {
				return (c - a).cross(-a).cross(c - a);
			}

			if (SameDirection(b - a, -a)) {
				return (b - a).cross(-a).cross(b - a);
			}

			return -a;
		}

		if (SameDirection((b - a).cross(abc), -a)) {
			if (SameDirection(b - a, -a)) {
				return (b - a).cross(-a).cross(b - a);
			}

			return -a;
		}

		if (SameDirection(abc, -a)) {
			return abc;
		}

		return -abc;
	}

	// Trapezoid
	iwmath::vector3 Simplex(
		const iwmath::vector3 & a,
		const iwmath::vector3 & b,
		const iwmath::vector3 & c,
		const iwmath::vector3 & d)
	{
		iwmath::vector3 abd = (b - a).cross(d - a);
		iwmath::vector3 bcd = (c - b).cross(d - c);
		iwmath::vector3 cad = (a - c).cross(d - a);

		if (SameDirection(abd, -a)) {
			iwmath::vector3 dir = Simplex(a, b, d);
			if (abd != -dir) {
				return dir;
			}
		}

		else if (SameDirection(bcd, -a)) {
			iwmath::vector3 dir = Simplex(b, c, d);
			if (bcd != -dir) {
				return dir;
			}
		}

		else if (SameDirection(cad, -a)) {
			iwmath::vector3 dir = Simplex(c, a, d);
			if (cad != -dir) {
				return dir;
			}
		}

		return 0;
	}
}
