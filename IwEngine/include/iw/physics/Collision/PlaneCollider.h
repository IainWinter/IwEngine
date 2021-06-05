#pragma once

#include "iw/physics/impl/PlaneCollider.h"

namespace iw {
	namespace Physics {
		using Line  = impl::PlaneCollider<d2>;
		using Plane = impl::PlaneCollider<d3>;
	}

	using namespace Physics;
}
