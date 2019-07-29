#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWUTIL_API __declspec(dllexport)
#else
#		define IWUTIL_API __declspec(dllimport)
#	endif
#endif

namespace iwu {}
#define iwutil iwu
