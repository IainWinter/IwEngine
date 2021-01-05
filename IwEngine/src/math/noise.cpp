#include "iw/math/noise.h"

namespace iw {
namespace math {
	// from wikipedia perlin noise

	vector2 randomGradient(
		int ix, int iy)
	{
	    float random = 2920.f
		            * sin(ix * 21942.f + iy * 171324.f + 8912.f)
		            * cos(ix * 23157.f * iy * 217832.f + 9758.f);

	    return vector2(cos(random), sin(random));
	}

	float dotGridGradient(
		int ix, int iy,
		float x, float y)
	{
	    vector2 gradient = randomGradient(ix, iy);
	    vector2 delta = vector2(x - ix, y - iy);

	    return delta.dot(gradient);
	}

	float perlin(float x, float y) {
		// Determine grid cell coordinates
		int x0 = (int)x;
		int y0 = (int)y;
		int x1 = x0 + 1;
		int y1 = y0 + 1;

		// Determine interpolation weights
		// Could also use higher order polynomial/s-curve here
		float sx = x - x0;
		float sy = y - y0;

		// Interpolate between grid point gradients
		float n0, n1, ix0, ix1;

		n0 = dotGridGradient(x0, y0, x, y);
		n1 = dotGridGradient(x1, y0, x, y);
		ix0 = lerp(n0, n1, sx);

		n0 = dotGridGradient(x0, y1, x, y);
		n1 = dotGridGradient(x1, y1, x, y);
		ix1 = lerp(n0, n1, sx);

		return lerp(ix0, ix1, sy);
	}
}
}
