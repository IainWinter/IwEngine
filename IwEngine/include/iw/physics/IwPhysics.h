#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWPHYSICS_API __declspec(dllexport)
#else
#		define IWPHYSICS_API __declspec(dllimport)
#	endif
#endif

#include <type_traits>

namespace IW {
namespace Physics {
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
