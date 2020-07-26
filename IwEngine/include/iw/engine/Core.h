#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWENGINE_DLL
#		define IWENGINE_API __declspec(dllexport)
#else
#		define IWENGINE_API __declspec(dllimport)
#	endif
#endif

#ifdef IW_DEBUG
	#define IF_DEBUG(x) x
#else
	#define IF_DEBUG(x)
#endif
