#pragma once

#include "IwEntity.h"
#include "EntityArchetype.h"

namespace IwEntity2 {
	struct Entity {
		EntityIndex          Index;
		EntityArchetype Archetype;
		unsigned int    Version;
		bool            Alive;

		Entity(
			EntityIndex id)
			: Index(id)
			, Archetype(0)
			, Version(0)
			, Alive(false)
		{}
	};
}
