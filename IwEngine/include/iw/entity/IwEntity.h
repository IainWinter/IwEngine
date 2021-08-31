#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWENTITY_DLL
#		define IWENTITY_API __declspec(dllexport)
#else
#		define IWENTITY_API __declspec(dllimport)
#	endif
#endif

#include "iw/util/memory/ref.h"

#ifdef IW_USE_REFLECTION
#	include "iw\reflection\Reflect.h"
#else
#	define REFLECT
#	include <typeindex>
#endif

#pragma warning(disable : 4200)

namespace iw {
namespace ECS {
#ifdef IW_USE_REFLECTION
	using ComponentType = const iw::Type*;
#else
	using ComponentType = std::type_index;
#endif
}

	using namespace ECS;
}
