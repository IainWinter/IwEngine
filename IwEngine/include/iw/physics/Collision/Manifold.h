#pragma once

#include "iw/physics/Dynamics/Rigidbody.h"

namespace IW {
namespace Physics {
	struct Manifold {
		Rigidbody* BodyA;
		Rigidbody* BodyB;
		iw::vector3 A;
		iw::vector3 B;
		iw::vector3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;
	};
}

	using namespace Physics;
}
