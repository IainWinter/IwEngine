#pragma once

#include "iw/physics/Collision/Collider.h"
#include "iw/physics/Collision/ManifoldPoints.h"

namespace iw {
namespace Physics {
	// x = col impl
	// o = col no impl
	// _ = no col
	//
	//         | Sphere  | Capsule | Plane  | Hull | Mesh
	// Sphere  |    x    |    x    |    x   |   x  |  x
	// Capsule |         |    o    |    o   |   o  |  o
	// Plane   |         |         |    _   |   o  |  o
	// Hull    |         |         |        |   x  |  x
	// Mesh    |         |         |        |      |  x
	ManifoldPoints TestCollision(
		Collider* a, Transform* at,
		Collider* b, Transform* bt);
}

	using namespace Physics;
}
