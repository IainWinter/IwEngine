#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct ComponentData {
		EntityArchetype Archetype;
		size_t Count;

	};
}
