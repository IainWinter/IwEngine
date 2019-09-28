#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "ComponentData.h"

namespace IwEntity {
	struct Entity {
		size_t Index;

		short Version;
		//bool  Alive;

		//iwu::ref<Archetype2> Archetype;
		//iwu::ref<ComponentData>    Components;

		Entity(
			size_t index)//,
			//iwu::ref<Archetype2> archetype)
			: Index(index)
			//, Archetype(archetype)
			, Version(0)
			//, Alive(true)
		{}
	};
}
