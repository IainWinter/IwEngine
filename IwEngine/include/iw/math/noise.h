#pragma once

#include "iw/math/iwmath.h"

namespace iw {
namespace math {
	/**
	* Compute Perlin noise at coordinates x, y, z
	* Returns a random float 0 - 1
	*/
	IWMATH_API
	float perlin(
		float x,
		float y = 0,
		float z = 0);
}

	using namespace math;
}
