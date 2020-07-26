#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWASSET_DLL
#		define IWASSET_API __declspec(dllexport)
#else
#		define IWASSET_API __declspec(dllimport)
#	endif
#endif
