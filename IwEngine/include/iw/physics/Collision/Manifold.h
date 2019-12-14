#pragma once

#include "iw/math/vector3.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/physics/IwPhysics.h"

namespace IW {
namespace Physics {
	struct Manifold {
		iw::vector3 A;
		iw::vector3 B;
		const Rigidbody *BodyA;
		const Rigidbody *BodyB;
		scalar PenetrationDepth;
	};
}

	using namespace Physics;
}
