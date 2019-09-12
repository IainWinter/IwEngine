#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct Chunk;

	struct ComponentData {
		//Chunk* Chunk;
		size_t ChunkIndex;
		void*  Components[];
	};
}
