#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
#include "Archetype.h"

namespace IW {
namespace ECS {
	struct EntityData {
		EntityHandle Entity;
		size_t ChunkIndex;
		iw::ref<Archetype> Archetype;
	};
}

	using namespace ECS;
}
