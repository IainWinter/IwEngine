#pragma once
#include "Collider.h"
#include "iw/math/vector3.h"

namespace IW {
inline namespace Physics {
	struct BoxCollider
		: Collider<iw::vector3>
	{
		BoxCollider(
			iw::vector3 center,
			float scale);
	};
}
}
