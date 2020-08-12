#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/Collider.h"
#include <array>

namespace iw {
namespace Physics {
namespace algo {
	bool GJK(
		const Collider* colliderA,
		const Collider* colliderB);

namespace detail {
	iw::vector3 Support(
		const Collider* colliderA,
		const Collider* colliderB,
		const iw::vector3& direction);

	bool NextSimplex(
		std::array<vector3, 4>& points,
		vector3& direction);

	bool Line(
		std::array<vector3, 4>& points,
		vector3& direction);

	bool Triangle(
		std::array<vector3, 4>& points,
		vector3& direction);

	bool Tetrahedron(
		std::array<vector3, 4>& points,
		vector3& direction);

	bool SameDirection(
		const iw::vector3& direction,
		const iw::vector3& ao);
}
}
}

	using namespace Physics;
}
