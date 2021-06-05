#pragma once

#include "iw/physics/impl/HullCollider.h"

namespace iw {
namespace Physics {
	using Hull2 = impl::HullCollider<d2>;
	using Hull  = impl::HullCollider<d3>;
}

	using namespace Physics;
}
