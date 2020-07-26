#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWUTIL_DLL
#		define IWUTIL_API __declspec(dllexport)
#else
#		define IWUTIL_API __declspec(dllimport)
#	endif
#endif

#include "iw/common/coredef.h"
