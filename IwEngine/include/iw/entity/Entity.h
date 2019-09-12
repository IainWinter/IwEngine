#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "ComponentData.h"

namespace IwEntity {
	struct Entity2 {
		size_t Index;

		short Version;
		bool  Alive;

		iwu::ref<const Archetype2> Archetype;
		iwu::ref<ComponentData>    ComponentData;

		Entity2(
			size_t index,
			iwu::ref<const Archetype2> archetype)
			: Index(index)
			, Archetype(archetype)
			, Version(0)
			, Alive(true)
		{}
	};
}
