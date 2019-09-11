#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct Chunk;

	struct ComponentData {
		Chunk* Chunk;
		void*  Components[];
	};
}
