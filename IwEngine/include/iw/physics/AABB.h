#pragma once

#include "iw/physics/impl/AABB.h"

namespace iw {
namespace Physics {
	using AABB2 = impl::AABB<d2>;
	using AABB  = impl::AABB<d3>;
}

	using namespace Physics;
}
