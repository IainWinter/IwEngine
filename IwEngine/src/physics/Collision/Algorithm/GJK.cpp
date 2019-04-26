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

		points[0] = Support(collider1, collider2, transformation1, 
			transformation2, iwm::vector3::unit_x);

		iwm::vector3 direction = -points[0];

		bool colliding = false;
		int count = 1;

		while (!colliding) {
			iwm::vector3 a = Support(collider1, collider2, transformation1,
				transformation2, direction);

			if (a.dot(direction) <= 0) {
				break;
			}

			points[count++] = a;

			//Simplex checks
			switch (count) {
				case 2: direction = Simplex(a, points[0]);            break;
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
						count--;
					}
					
					break;
				}
			}
		}


		return colliding;
	}

	iwm::vector3 Support(
		const Collider& collider1,
		const Collider& collider2,
		const iwm::matrix4& transformation1,
		const iwm::matrix4& transformation2,
		const iwm::vector3& direction)
	{
		return collider1.FurthestPoint(transformation1, direction)
		     - collider2.FurthestPoint(transformation2, -direction);
	}

	//Line
	iwm::vector3 Simplex(
		const iwm::vector3& a,
		const iwm::vector3& b)
	{
		if (SameDirection(b - a, -a)) {
			return (b - a).cross(-a).cross(b - a);
		}
		else {
			return -a;
		}
	}

	//Triangle
	iwm::vector3 Simplex(
		const iwm::vector3 & a,
		const iwm::vector3 & b,
		const iwm::vector3 & c)
	{
		iwm::vector3 abc = (b - a).cross(c - a);
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

	//Trapezoid
	iwm::vector3 Simplex(
		const iwm::vector3 & a,
		const iwm::vector3 & b,
		const iwm::vector3 & c,
		const iwm::vector3 & d)
	{
		iwm::vector3 abd = (b - a).cross(d - a);
		iwm::vector3 bcd = (c - b).cross(d - c);
		iwm::vector3 cad = (a - c).cross(d - a);

		if (SameDirection(abd, -a)) {
			iwm::vector3 dir = Simplex(a, b, d);
			if (abd != -dir) {
				return dir;
			}
		}

		else if (SameDirection(bcd, -a)) {
			iwm::vector3 dir = Simplex(b, c, d);
			if (bcd != -dir) {
				return dir;
			}
		}

		else if (SameDirection(cad, -a)) {
			iwm::vector3 dir = Simplex(c, a, d);
			if (cad != -dir) {
				return dir;
			}
		}

		return 0;
	}
}
