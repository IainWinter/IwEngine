#pragma once

#include "iw/physics/IwPhysics.h"

namespace iw {
namespace Physics {
	struct ManifoldPoints {
		iw::vector3 A;
		iw::vector3 B;
		iw::vector3 Normal;
		scalar PenetrationDepth;
		bool HasCollision;
	};
}

	using namespace Physics;
}
