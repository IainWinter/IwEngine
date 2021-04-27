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
	};
}

	using namespace Physics;
}
