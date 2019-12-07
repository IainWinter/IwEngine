#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include "ComponentData.h"

namespace IW {
namespace ECS {
	struct EntityComponentData {
		// Entity

		size_t& Index;
		short&  Version;

		ComponentData& Components;
	};
}

	using namespace ECS;
}
