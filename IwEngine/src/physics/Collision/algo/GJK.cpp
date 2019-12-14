#include "iw/physics/Collision/algo/GJK.h"
#include <vector>

namespace IW {
namespace Physics {
namespace algo {
	bool GJK(
		const CollisionObject* a,
		const CollisionObject* b)
	{
		iw::vector3 points[4];

		points[0] = detail::Support(a, b, iw::vector3::unit_x);

		iw::vector3 direction = -points[0];

		bool colliding = false;
		int tests = 0;
		int count = 1;

		while (!colliding && tests < 5) {
			iw::vector3 p = detail::Support(a, b, direction);

			if (p.dot(direction) <= 0) {
				break;
			}

			tests++;
			points[count++] = p;

			switch (count) {
				case 2: direction = detail::Simplex(p, points[0]);            break;
				case 3: direction = detail::Simplex(p, points[1], points[0]); break;
				case 4: {
					direction = detail::Simplex(p, points[2], points[1], points[0]);
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

namespace detail {
	iw::vector3 Support(
		const CollisionObject* a,
		const CollisionObject* b,
		const iw::vector3& direction)
	{
		return 0/*a->Col()->FurthestPoint(direction)
			 - b->Col()->FurthestPoint(direction)*/;
	}

	//Line
	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b)
	{
		if (SameDirection(b - a, -a)) {
			return (b - a).cross(-a).cross(b - a);
		}

		return -a;
	}

	//Triangle
	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b,
		const iw::vector3& c)
	{
		iw::vector3 abc = (b - a).cross(c - a);
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
	iw::vector3 Simplex(
		const iw::vector3& a,
		const iw::vector3& b,
		const iw::vector3& c,
		const iw::vector3& d)
	{
		iw::vector3 abd = (b - a).cross(d - a);
		iw::vector3 bcd = (c - b).cross(d - c);
		iw::vector3 cad = (a - c).cross(d - a);

		if (SameDirection(abd, -a)) {
			iw::vector3 dir = Simplex(a, b, d);
			if (abd != -dir) {
				return dir;
			}
		}

		if (SameDirection(bcd, -a)) {
			iw::vector3 dir = Simplex(b, c, d);
			if (bcd != -dir) {
				return dir;
			}
		}

		if (SameDirection(cad, -a)) {
			iw::vector3 dir = Simplex(c, a, d);
			if (cad != -dir) {
				return dir;
			}
		}

		return 0;
	}

	bool SameDirection(
		const iw::vector3& direction,
		const iw::vector3& ao)
	{
		return direction.dot(ao) > 0;
	}
}
}
}
}
