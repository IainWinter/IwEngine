#pragma once

#include "iw/physics/impl/SphereCollider.h"

namespace iw {
namespace Physics {
	using Circle = impl::SphereCollider<d2>;
	using Sphere = impl::SphereCollider<d3>;
}

	using namespace Physics;
}
