#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWECS_API __declspec(dllexport)
#else
#		define IWECS_API __declspec(dllimport)
#	endif
#endif

namespace iwecs {}