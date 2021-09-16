#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
#include "Archetype.h"

namespace iw {
namespace ECS {
	struct EntityData {
		EntityHandle Entity;
		Archetype Archetype;
		size_t ChunkIndex;
	};
}

	using namespace ECS;
}
