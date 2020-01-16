#include "iw/entity/Chunk.h"
#include <iw\log\logger.h>

namespace IW {
	size_t Chunk::ReserveComponents() {
		++Count;
		return CurrentIndex++ + IndexOffset;
	}

	void Chunk::ReinstateComponents() {
		++Count;
	}

	void Chunk::FreeComponents() {
		--Count;
	}

	bool Chunk::ContainsIndex(
		size_t index) const
	{
		return  IndexOffset <= index
			&& IndexOffset + CurrentIndex > index;
	}

	size_t Chunk::EndIndex() const {
		return CurrentIndex + IndexOffset;
	}

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
