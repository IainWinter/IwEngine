#pragma once

#include "iw/physics/impl/CapsuleCollider.h"

namespace iw {
namespace Physics {
	using Capsule2 = impl::CapsuleCollider<d2>;
	using Capsule  = impl::CapsuleCollider<d3>;
}

	using namespace Physics;
}
