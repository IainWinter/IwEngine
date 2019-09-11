#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "ComponentData.h"

namespace IwEntity {
	struct Entity2 {
		size_t Index;
		size_t ChunkIndex;

		short Version;
		bool  Alive;

		std::weak_ptr<Archetype2>      Archetype;
		std::shared_ptr<ComponentData> ComponentData;

		Entity2(
			size_t index,
			std::weak_ptr<Archetype2> archetype)
			: Index(index)
			, Archetype(archetype)
			, Version(0)
			, Alive(true)
		{}
	};
}
