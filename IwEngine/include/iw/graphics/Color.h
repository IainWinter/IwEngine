#pragma once

#include "IwGraphics.h"
#include "iw/math/vector4.h"

namespace IW {
namespace Graphics {
	struct Color {
		float r, g, b, a;

		Color()
			: r(0)
			, g(0)
			, b(0)
			, a(0)
		{}

		Color(
			float r,
			float g,
			float b)
			: r(r)
			, g(g)
			, b(b)
			, a(1.0f)
		{}

		Color(
			float r,
			float g,
			float b,
			float a)
			: r(r)
			, g(g)
			, b(b)
			, a(a)
		{}

		operator iw::vector4() const {
			return iw::vector4(r, g, b, a);
		}
	};
}

	using namespace Graphics;
}
