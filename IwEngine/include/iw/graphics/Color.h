#pragma once

#include "IwGraphics.h"
#include "iw/math/vector4.h"

namespace iw {
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
			float rgba)
			: r(rgba)
			, g(rgba)
			, b(rgba)
			, a(rgba)
		{}

		Color(
			float r,
			float g,
			float b,
			float a = 1.0f)
			: r(r)
			, g(g)
			, b(b)
			, a(a)
		{}

		operator iw::vector4() {
			return iw::vector4(r, g, b, a);
		}

		operator iw::vector4() const {
			return iw::vector4(r, g, b, a);
		}

		static Color From255(
			int r,
			int g,
			int b,
			int a = 255) {
			return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		//static Color Blend(
		//	const Color& a,
		//	const Color& b,
		//	float amount) // with mode
		//{
		//	return lerp(a, b, amount);
		//}
	};
}

	using namespace Graphics;
}
