#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWDATA_API __declspec(dllexport)
#else
#		define IWDATA_API __declspec(dllimport)
#	endif
#endif
