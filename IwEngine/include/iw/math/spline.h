#pragma once

#include "iw/math/iwmath.h"

namespace iw
{
namespace math
{
	template<typename _t>
	struct spline
	{
		_t A;
		_t B;
		_t C;

		_t F(float w)
		{
			_t a = iw::lerp<_t>(A, B, w);
			_t b = iw::lerp<_t>(B, C, w);
			_t c = iw::lerp<_t>(a, b, w);

			return c;
		}
	};

	using spline2 = spline<glm::vec2>;
	using spline3 = spline<glm::vec3>;
}
	using namespace math;
}
