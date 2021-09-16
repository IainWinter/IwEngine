#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWGRAPHICS_DLL
#		define IWGRAPHICS_API __declspec(dllexport)
#else
#		define IWGRAPHICS_API __declspec(dllimport)
#	endif
#else
#	define IWGRAPHICS_API
#endif

#include "iw/common/coredef.h"
