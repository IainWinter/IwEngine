#pragma once

#include "iw/physics/IwPhysics.h"

namespace iw {
namespace Physics {
	struct ManifoldPoints {
		glm::vec3 A;
		glm::vec3 B;
		glm::vec3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;

		ManifoldPoints()
			: A(0.0f)
			, B(0.0f)
			, Normal(0.0f)
			, PenetrationDepth(0.0f)
			, HasCollision(false)
		{}

		ManifoldPoints(
			glm::vec3 a,
			glm::vec3 b,
			glm::vec3 n,
			float     d
		)
			: A(a)
			, B(b)
			, Normal(n)
			, PenetrationDepth(d)
			, HasCollision(true)
		{}

		ManifoldPoints(
			glm::vec2 a,
			glm::vec2 b,
			glm::vec2 n,
			float     d
		)
			: A(a, 0)
			, B(b, 0)
			, Normal(n, 0.0f)
			, PenetrationDepth(d)
			, HasCollision(true)
		{}
	};
}

	using namespace Physics;
}
