#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWPHYSICS_API __declspec(dllexport)
#else
#		define IWPHYSICS_API __declspec(dllimport)
#	endif
#endif

#include <type_traits>
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace IW {
namespace Physics {

#ifdef IW_DOUBLE_PRECISION
	using scalar = long double;
#else
	using scalar = float;
#endif

namespace impl {
	template<
		typename V>
	constexpr int GetNumPoints() {
		if constexpr (std::is_same_v<iw::vector2, V>) {
			return 2;
		}

		if constexpr (std::is_same_v<iw::vector3, V>) {
			return 3;
		}

		if constexpr (std::is_same_v<iw::vector4, V>) {
			return 4;
		}
	}
}
}
}
