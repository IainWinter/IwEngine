#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWENTITY_API __declspec(dllexport)
#else
#		define IWENTITY_API __declspec(dllimport)
#	endif
#endif

#include "iw/util/type/family.h"

namespace IwEntity {
	struct ComponentFamilyTag {};

	using Entity          = unsigned int;
	using ComponentId     = unsigned int;
	using Archetype       = unsigned int;
	using ComponentFamily = iwu::family<ComponentFamilyTag>;

	template<
		typename... _cs>
	Archetype GetArchetype() {
		return ((1 << ComponentFamily::type<_cs>) | ...);
	}

	inline bool Similar(
		Archetype a1,
		Archetype a2)
	{
		return (a1 & a2) == a2;
	}
}
