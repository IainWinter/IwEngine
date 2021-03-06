#pragma once

#include "IwGraphics.h"
#include "iw/math/vector3.h"
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

		Color(
			iw::vector4 rgba)
			: r(rgba.x)
			, g(rgba.y)
			, b(rgba.z)
			, a(rgba.w)
		{}

		iw::vector3 rgb() const {
			return iw::vector3(r, g, b);
		}

		operator iw::vector4() {
			return iw::vector4(r, g, b, a);
		}

		operator iw::vector4() const {
			return iw::vector4(r, g, b, a);
		}

		operator unsigned int() const {
			return unsigned int(r * 255u) << 0
				| unsigned int(g * 255u) << 8
				| unsigned int(b * 255u) << 16
				| unsigned int(a * 255u) << 24;
		}

		static Color From255(
			int r,
			int g,
			int b,
			int a = 255)
		{
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
