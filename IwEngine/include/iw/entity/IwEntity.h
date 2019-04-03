#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWENTITY_API __declspec(dllexport)
#else
#		define IWENTITY_API __declspec(dllimport)
#	endif
#endif

namespace IwEntity {
	using Entity      = unsigned int;
	using ComponentId = unsigned int;
	using Archetype   = unsigned int;
}
