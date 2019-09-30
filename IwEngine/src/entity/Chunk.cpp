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

	bool Chunk::IsLast(
		size_t index)
	{
		return index == LastIndex();
	}

	size_t Chunk::LastIndex() {
		return EntityIndex + CurrentIndex - 1;
	}

	Entity* Chunk::GetEntity(
		size_t index)
	{
		return (Entity*)Buffer + index - EntityIndex;
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
			* (index - EntityIndex);
	}
}
