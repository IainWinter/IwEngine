#pragma once

#include "IwEntity.h"

namespace IwEntity2 {
	struct EntityData {
		Entity          Id;
		EntityArchetype Archetype;
		unsigned int    Version;
		bool            Alive;

		EntityData(
			Entity id)
			: Id(id)
			, Archetype(0)
			, Version(0)
			, Alive(false)
		{}
	};
}
