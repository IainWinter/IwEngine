#include "GJK.h"
#include <vector>
#include "IwMath\vector3.h"

iwphysics::collision_data* iwphysics::GJK(
	const collider* collider1,
	const collider* collider2,
	const collision_transformation& collisionTrans)
{
	iwmath::vector3* points = new iwmath::vector3[4];
	iwmath::vector3 direction = iwmath::vector3::unitX;

	iwmath::vector3 supportVec = support(collider1, collider2, collisionTrans, direction);
	points[0] = supportVec;
	direction = -supportVec;

	bool colliding = false;
	int pointCount = 1;

	while (!colliding) {
		iwmath::vector3 a = support(collider1, collider2, collisionTrans, direction);

		if (a.dot(direction) <= 0) {
			break;
		}

		pointCount++;
		points[pointCount - 1] = a;

		//Simplex checks
		switch (pointCount) {
		case 2: direction = simplex(a, points[0]); break;
		case 3: direction = simplex(a, points[1], points[0]); break;
		case 4: {
			direction = simplex(a, points[2], points[1], points[0]);
			if (direction == 0) {
				colliding = true;
			}
			else {
				points[0] = points[1];
				points[1] = points[2];
				points[2] = points[3];
				delete& points[3];
			}

			break;
		}
		}
	}

	return new collision_data(collider1, collider2, /*poi*/iwmath::vector3());
}

iwmath::vector3 iwphysics::support(
	const collider* bounds1,
	const collider* bounds2,
	const collision_transformation& collisionTrans,
	const iwmath::vector3& direction)
{
	return bounds1->support(direction, collisionTrans.transformation1->translation(), collisionTrans.transformation1->rotation())
		- bounds2->support(direction, collisionTrans.transformation2->translation(), collisionTrans.transformation2->rotation());
}

//Line
iwmath::vector3 iwphysics::simplex(
	const iwmath::vector3& a,
	const iwmath::vector3& b)
{
	if (is_same_direction(b - a, -a)) {
		return (b - a).cross(-a).cross(b - a);
	}
	else {
		return -a;
	}
}

//Triangle
iwmath::vector3 iwphysics::simplex(
	const iwmath::vector3 & a,
	const iwmath::vector3 & b,
	const iwmath::vector3 & c)
{
	iwmath::vector3 abc = (b - a).cross(c - a);
	if (is_same_direction(abc.cross(c - a), -a)) {
		if (is_same_direction(c - a, -a)) {
			return (c - a).cross(-a).cross(c - a);
		}

		if (is_same_direction(b - a, -a)) {
			return (b - a).cross(-a).cross(b - a);
		}

		return -a;
	}

	if (is_same_direction((b - a).cross(abc), -a)) {
		if (is_same_direction(b - a, -a)) {
			return (b - a).cross(-a).cross(b - a);
		}

		return -a;
	}

	if (is_same_direction(abc, -a)) {
		return abc;
	}

	return -abc;
}

//Trapazoid
iwmath::vector3 iwphysics::simplex(
	const iwmath::vector3 & a,
	const iwmath::vector3 & b,
	const iwmath::vector3 & c,
	const iwmath::vector3 & d)
{
	iwmath::vector3 abd = (b - a).cross(d - a);
	iwmath::vector3 bcd = (c - b).cross(d - c);
	iwmath::vector3 cad = (a - c).cross(d - a);

	if (is_same_direction(abd, -a)) {
		iwmath::vector3 dir = simplex(a, b, d);
		if (abd != -dir) {
			return dir;
		}
	}
	else if (is_same_direction(bcd, -a)) {
		iwmath::vector3 dir = simplex(b, c, d);
		if (bcd != -dir) {
			return dir;
		}
	}
	else if (is_same_direction(cad, -a)) {
		iwmath::vector3 dir = simplex(c, a, d);
		if (cad != -dir) {
			return dir;
		}
	}

	return 0;
}
