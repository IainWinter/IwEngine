#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
#include "ComponentData.h"

namespace iw {
namespace ECS {
	struct EntityComponentData {
		EntityHandle Handle;

		size_t Index; // dep
		short Version;

		ComponentData& Components;
	};
}

	using namespace ECS;
}
