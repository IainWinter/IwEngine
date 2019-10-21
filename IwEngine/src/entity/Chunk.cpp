#include "iw/entity/Chunk.h"
#include <iw\log\logger.h>

namespace IwEntity {
	size_t Chunk::BeginIndex() {
		size_t i = IndexOffset;
		while (i != EndIndex() && !GetEntity(i)->Alive) {
			i++;
		}

		return i;
	}
	
	Entity* Chunk::GetEntity(
		size_t index)
	{
		return (Entity*)Buffer + index - IndexOffset;
	}

	char* Chunk::GetComponentStream(
		const ArchetypeLayout& layout)
	{
		return Buffer + (sizeof(Entity) + layout.Offset) * Capacity;
	}

	char* Chunk::GetComponentData(
		const ArchetypeLayout& layout,
		size_t index)
	{
		return GetComponentStream(layout)
			+ layout.Component->Size
			* (index - IndexOffset);
	}
}
