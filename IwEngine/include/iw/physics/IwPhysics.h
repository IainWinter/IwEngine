#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWPHYSICS_DLL
#		define IWPHYSICS_API __declspec(dllexport)
#else
#		define IWPHYSICS_API __declspec(dllimport)
#	endif
#else
#	define IWPHYSICS_API
#endif

#include "iw/math/iwmath.h"
#include "iw/util/reflection/ReflectDef.h"

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
