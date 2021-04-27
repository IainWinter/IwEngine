#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWPHYSICS_DLL
#		define IWPHYSICS_API __declspec(dllexport)
#else
#		define IWPHYSICS_API __declspec(dllimport)
#	endif
#endif

#include <type_traits>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace iw {
namespace Physics {

#ifdef IW_DOUBLE_PRECISION
	using scalar = long double;
#else
	using scalar = float;
#endif

}

	using namespace Physics;
}

// for templates?

#ifdef IW_USE_REFLECTION
#	include "iw\reflection\Reflect.h"
#else
#	ifndef REFLECT
#		define REFLECT
#	endif
#endif

#ifdef REFLECTING
#	undef  IWPHYSICS_API
#	define IWPHYSICS_API
#endif


inline float major(glm::vec2& v) {
	float m = v.x;
	if (v.y > m) m = v.y;
	return m;
}

inline float major(glm::vec3& v) {
	float m = v.x;
	if (v.y > m) m = v.y;
	if (v.z > m) m = v.z;
	return m;
}

inline float minor(glm::vec3& v) {
	float m = v.x;
	if (v.y < m) m = v.y;
	if (v.z < m) m = v.z;
	return m;
}
