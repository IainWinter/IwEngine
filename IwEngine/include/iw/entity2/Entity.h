#pragma once

#include "IwEntity.h"
#include "EntityArchetype.h"

namespace IwEntity2 {
	struct Entity {
		EntityIndex     Index;
		EntityArchetype Archetype;
		unsigned int    Version;
		bool            Alive;
		char*           Components;

		Entity(
			EntityIndex index)
			: Index(index)
			, Archetype(0)
			, Version(0)
			, Alive(false)
		{}
	};
}
