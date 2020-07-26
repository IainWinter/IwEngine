#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWEVENTS_DLL
#		define IWEVENTS_API __declspec(dllexport)
#else
#		define IWEVENTS_API __declspec(dllimport)
#	endif
#endif
