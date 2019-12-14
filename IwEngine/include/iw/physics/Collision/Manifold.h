#pragma once

#include "iw/math/vector3.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/physics/IwPhysics.h"

namespace IW {
namespace Physics {
	struct Manifold {
		const Rigidbody* BodyA;
		const Rigidbody* BodyB;
		iw::vector3 A;
		iw::vector3 B;
		scalar PenetrationDepth;
	};
}

	using namespace Physics;
}
