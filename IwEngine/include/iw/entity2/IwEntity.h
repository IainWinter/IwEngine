#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWENTITY2_API __declspec(dllexport)
#else
#		define IWENTITY2_API __declspec(dllimport)
#	endif
#endif

namespace IwEntity2 {
	using ComponentType   = unsigned int;
	using EntityIndex     = unsigned int;
}