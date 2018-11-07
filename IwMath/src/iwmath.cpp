#include "iwmath.h"
#include <cmath>

namespace iwmath {
	float inv_sqrt(float x) {
		float xhalf = 0.5f * x;
		int i = *(int*)&x;
		i = 0x5f375a86 - (i >> 1);
		x = *(float*)&i;
		x = x * (1.5f - xhalf * x * x);
		return x;
	}

	bool almost_equal(float a, float b, unsigned int accuracy) {
		float dif = fabsf(a - b);
		return dif * pow(10, accuracy) < 1;
	}
}