#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWUI_API __declspec(dllexport)
#else
#		define IWUI_API __declspec(dllimport)
#	endif
#endif

//#include "iw/common/coredef.h"
