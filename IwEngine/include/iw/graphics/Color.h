#pragma once

#include "IwGraphics.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace iw {
namespace Graphics {
	struct Color {
		float r, g, b, a;

		Color() : r(0), g(0), b(0), a(0) {}

		Color(float r, float g, float b, float a = 1.0f) : r(r),      g(g),      b(b),      a(a)      {}
		Color(float rgba)                                : r(rgba),   g(rgba) ,  b(rgba),   a(rgba)   {}
		Color(vector3 rgb)                               : r(rgb .x), g(rgb .y), b(rgb .z), a(1)      {}
		Color(vector4 rgba)                              : r(rgba.x), g(rgba.y), b(rgba.z), a(rgba.w) {}

		vector2 rg()   const { return vector3(r, g); }
		vector3 rgb()  const { return vector3(r, g, b); }
		vector4 rgba() const { return vector4(r, g, b, a); }

		uint32_t to32() const {
			return clamp<unsigned>(r * 255, 0, 255) << 0
				| clamp<unsigned>(g * 255, 0, 255) << 8
				| clamp<unsigned>(b * 255, 0, 255) << 16
				| clamp<unsigned>(a * 255, 0, 255) << 24; // Not sure if this should be clamped or let to explode?
		}

		Color operator+(
			const Color& other) const
		{
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
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
