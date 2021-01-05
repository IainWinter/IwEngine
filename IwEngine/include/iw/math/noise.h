#pragma once

#include "vector2.h"

namespace iw {
namespace math {
	/**
	* Compute Perlin noise at coordinates x, y.
	* Returns a random float -1 - 1
	*/
	IWMATH_API
	float perlin(float x, float y);
}

	using namespace math;
}
