#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWAUDIO_DLL
#		define IWAUDIO_API __declspec(dllexport)
#else
#		define IWAUDIO_API __declspec(dllimport)
#	endif
#endif
