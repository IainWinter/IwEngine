#include "iw/entity/Chunk.h"

namespace IwEntity {
	size_t Chunk::ReserveComponents() {
		++Count;
		return CurrentIndex++ + EntityIndex;
	}

	void Chunk::ReinstateComponents() {
		++Count;
	}

	void Chunk::FreeComponents() {
		--Count;
	}

	bool Chunk::IsEnd(
		size_t index)
	{
		return index == EndIndex();
	}

	size_t Chunk::BeginIndex() {
		size_t i = 0;
		while (i != EndIndex() && !GetEntity(i)->Alive) {
			i++;
		}

		return i;
	}

	size_t Chunk::EndIndex() {
		return EntityIndex + CurrentIndex;
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
