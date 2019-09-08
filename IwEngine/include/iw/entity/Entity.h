#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct Entity2 {
		size_t Index;
		EntityArchetype Archetype;

		Entity2(
			size_t index,
			const EntityArchetype& archetype)
			: Index(index)
			, Archetype(archetype)
		{}
	};
}
