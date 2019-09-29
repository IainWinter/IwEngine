#include "iw/entity/Chunk.h"

namespace IwEntity {
	size_t Chunk::ReserveComponents() {
		++Count;
		return CurrentIndex++ + EntityIndex;
	}

	void Chunk::ReinstateComponents() {
		Count++;
	}

	void Chunk::FreeComponents() {
		--Count;
	}

	Entity* Chunk::GetEntity(
		size_t index)
	{
		return (Entity*)Buffer + index - EntityIndex;
	}

	char* Chunk::GetComponentStream(
		size_t capacity,
		const ArchetypeLayout& layout)
	{
		return Buffer + (sizeof(Entity) + layout.Offset) * capacity;
	}
}
