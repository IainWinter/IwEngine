#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWCOMMON_DLL
#		define IWCOMMON_API __declspec(dllexport)
#else
#		define IWCOMMON_API __declspec(dllimport)
#	endif
#endif

#ifdef IW_USE_REFLECTION
#	include "iw\reflection\Reflect.h"
#else
#	ifndef REFLECT
#		define REFLECT
#	endif
#endif

#ifdef REFLECTING
#	undef  IWCOMMON_API
#	define IWCOMMON_API
#endif

// Put into IwCommon

#include "iw/math/matrix.h" // should go in a translation type file
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"

namespace iw {
#ifdef IW_DOUBLE_PRECISION
	using scalar = long double;
#else
	using scalar = float;
#endif

	enum /*class*/ Dimension {
		d2 = 2,
		d3 = 3,
	};

	template<Dimension _d>
	using _vec = glm::vec<size_t(_d), scalar>;

namespace math_translation {
	inline vec<3> from_glm(glm::vec3 v) {
		vec<3> result;

		result.set(0, v.x);
		result.set(1, v.y);
		result.set(2, v.z);

		return result;
	}

	inline mat<3, 3> from_glm(glm::mat3 v) {
		mat<3, 3> result;

		for (int i = 0; i < 9; i++) result.elements[i] = v[i / 3][i % 3];

		return result;
	}

	inline glm::vec3 to_glm(vec<3> vec) {
		return glm::vec3(vec.get(0), vec.get(1), vec.get(2));
	}

	inline glm::vec2 to_glm(vec<2> vec) {
		return glm::vec2(vec.get(0), vec.get(1));
	}

	inline float cross_length(glm::vec2 a, glm::vec2 b) { // should move to geom 2d
		return a.x * b.y - a.y * b.x;
	}
}

	using namespace math_translation;
}
