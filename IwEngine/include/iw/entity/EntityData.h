#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include "Archetype.h"

namespace IwEntity {
	struct EntityData {
		Entity Entity;
		size_t ChunkIndex;
		iwu::ref<Archetype> Archetype;
	};
}
