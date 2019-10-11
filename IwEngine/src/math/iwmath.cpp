#include "iw/math/iwmath.h"
#include <cmath>

namespace iwmath {
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
		return u.d;
	}

	bool almost_equal(
		float a,
		float b,
		unsigned int accuracy)
	{
		return fabsf(a - b) * fast_pow(10, accuracy) < 1; //didn't speed up much at all :c
	}
}
