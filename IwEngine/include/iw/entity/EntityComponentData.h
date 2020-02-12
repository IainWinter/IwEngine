#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
#include "ComponentData.h"

namespace iw {
namespace ECS {
	struct EntityComponentData {
		size_t& Index;
		short&  Version;

		ComponentData& Components;
	};
}

	using namespace ECS;
}
