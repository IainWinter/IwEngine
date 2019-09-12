#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWENTITY_API __declspec(dllexport)
#else
#		define IWENTITY_API __declspec(dllimport)
#	endif
#endif

#include "iw/util/type/family.h"
#include "iw/util/memory/smart_pointers.h"
#include <typeindex>

namespace IwEntity {
	struct ComponentFamilyTag {};

	using Archetype       = unsigned int;
	using Entity          = unsigned int;
	using EntityIndex     = unsigned int;
	using ComponentId     = unsigned int;
	using ComponentFamily = iwu::family<ComponentFamilyTag>;

	using ComponentType = std::type_index;
}
