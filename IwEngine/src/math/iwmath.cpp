#include "iw/math/iwmath.h"
#include <random>
#include <cmath>

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace iw {
namespace math {
	float randf() {
		return ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
	}

	int randi(
		int max)
	{
		return (randf()+1)/2 * max;
	}

	float fast_inv_sqrt(
		float x)
	{
		float xhalf = 0.5f * x;
		int i = *(int*)&x;
		i = 0x5f375a86 - (i >> 1);
		x = *(float*)&i;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}

	float fast_pow(
		float base, 
		float exponent)
	{
		union {
			double d;
			int x[2];
		} u = { base };
		u.x[1] = (int)(exponent * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;
		return (float)u.d;
	}

	bool almost_equal(
		float a,
		float b,
		unsigned int accuracy)
	{
		return fabsf(a - b) * fast_pow(10, (float)accuracy) < 1;
	}

	float ease(
		float time,
		float scale)
	{
		return 1 - (1 - time / scale) * (1 - time / scale);
	}

	template<>
	vector2 clamp(
		const vector2& v,
		const vector2& min,
		const vector2& max)
	{
		return vector2(
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y)
		);
	}
}

	using namespace math;
}
