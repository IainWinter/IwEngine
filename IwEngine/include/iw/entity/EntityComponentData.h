#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include "ComponentData.h"

namespace IwEntity {
	struct EntityComponentData {
		// Entity

		size_t& Index;
		short&  Version;

		ComponentData& Components;
	};
}
