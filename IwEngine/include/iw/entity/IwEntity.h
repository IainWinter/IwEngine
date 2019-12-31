#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWENTITY_API __declspec(dllexport)
#else
#		define IWENTITY_API __declspec(dllimport)
#	endif
#endif

#include "iw/util/memory/ref.h"
#include <typeindex>

namespace IW {
namespace ECS {
	using ComponentType = std::type_index;
}

	using namespace ECS;
}
