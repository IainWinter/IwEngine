#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWPHYSICS_DLL
#		define IWPHYSICS_API __declspec(dllexport)
#else
#		define IWPHYSICS_API __declspec(dllimport)
#	endif
#endif

#include <type_traits>
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

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

#include "iw/reflection/ReflectDef.h"

#ifdef REFLECTING
#	undef  IWPHYSICS_API
#	define IWPHYSICS_API
#endif
