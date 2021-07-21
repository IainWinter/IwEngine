#pragma once

#include "IwGraphics.h"
//#include "iw/math/vector2.h"
//#include "iw/math/glm::vec3.h"
//#include "iw/math/glm::vec4.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "glm/common.hpp"

namespace iw {
namespace Graphics {
	struct Color {
		float r, g, b, a;

		Color() : r(0), g(0), b(0), a(0) {}

		Color(float r, float g, float b, float a = 1.0f) : r(r),      g(g),      b(b),      a(a)      {}
		Color(float rgba)                                : r(rgba),   g(rgba) ,  b(rgba),   a(rgba)   {}
		Color(glm::vec3 rgb)                             : r(rgb .x), g(rgb .y), b(rgb .z), a(1)      {}
		Color(glm::vec4 rgba)                            : r(rgba.x), g(rgba.y), b(rgba.z), a(rgba.w) {}

		glm::vec2 rg()   const { return glm::vec2(r, g); }
		glm::vec3 rgb()  const { return glm::vec3(r, g, b); }
		glm::vec4 rgba() const { return glm::vec4(r, g, b, a); }

		uint32_t to32() const {
			return glm::clamp<unsigned>(r * 255, 0, 255) << 0
				| glm::clamp<unsigned>(g * 255, 0, 255) << 8
				| glm::clamp<unsigned>(b * 255, 0, 255) << 16
				| glm::clamp<unsigned>(a * 255, 0, 255) << 24; // Not sure if this should be clamped or let to explode?
		}

		Color operator+(
			const Color& other) const
		{
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}

		bool operator==(
			const Color& other) const
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		bool operator!=(
			const Color& other) const
		{
			return !operator==(other);
		}

		static Color From255(
			int r,
			int g,
			int b,
			int a = 255)
		{
			return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}
		
		static Color From32(
			uint32_t color)
		{
			return Color(
				(( color & (0xff << 0)  ) >> 0  ) / 255.0f,
				(( color & (0xff << 8)  ) >> 8  ) / 255.0f,
				(( color & (0xff << 16) ) >> 16 ) / 255.0f,
				(( color & (0xff << 24) ) >> 24 ) / 255.0f
			);
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
