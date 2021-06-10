#pragma once

#include "iw/physics/impl/MeshCollider.h"
#include "iw/physics/Collision/HullCollider.h"


namespace iw {
	namespace Physics {
		using MeshCollider2 = impl::MeshCollider<d2>;
		using MeshCollider  = impl::MeshCollider<d3>;
	}

	using namespace Physics;
}
