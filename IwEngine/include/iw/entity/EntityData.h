#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
#include "Archetype.h"

namespace iw {
namespace ECS {
	struct EntityData {
		EntityHandle Entity;
		size_t ChunkIndex;
		ref<Archetype> Archetype;
	};
}

	using namespace ECS;
}
