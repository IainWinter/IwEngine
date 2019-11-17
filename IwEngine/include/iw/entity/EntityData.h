#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include "Archetype.h"

namespace IW {
inline namespace ECS {
	struct EntityData {
		Entity Entity;
		size_t ChunkIndex;
		iw::ref<Archetype> Archetype;
	};
}
}
