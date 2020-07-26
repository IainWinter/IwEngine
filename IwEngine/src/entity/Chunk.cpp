#include "iw/entity/Chunk.h"
#include <iw\log\logger.h>

namespace iw {
namespace ECS {
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
		return IndexOffset <= index
			&& IndexOffset + CurrentIndex > index;
	}

	size_t Chunk::EndIndex() const {
		return CurrentIndex + IndexOffset;
	}

	size_t Chunk::BeginIndex() { // can def store this
		size_t i = IndexOffset;
		while (i != EndIndex() && !GetEntity(i)->Alive) {
			i++;
		}

		return i;
	}
	
	int Chunk::IndexOf(
		const ArchetypeLayout& layout,
		void* instance) const
	{
		const char* stream = GetComponentStream(layout);
		if (   stream <= instance
			&& stream + layout.Component->Size * CurrentIndex > instance)
		{
			size_t index = ((char*)instance - stream) / layout.Component->Size + IndexOffset;
			if (GetEntity(index)->Alive) {
				return (int)index;
			}
		}

		return -1;
	}

	EntityHandle* Chunk::GetEntity(
		size_t index)
	{
		return (EntityHandle*)Buffer + index - IndexOffset; // no copy again
	}

	const EntityHandle* Chunk::GetEntity(
		size_t index) const
	{
		return (EntityHandle*)Buffer + index - IndexOffset;
	}

	char* Chunk::GetComponentStream(
		const ArchetypeLayout& layout)
	{
		return Buffer + (sizeof(EntityHandle) + layout.Offset) * Capacity;
	}

	const char* Chunk::GetComponentStream(
		const ArchetypeLayout& layout) const
	{
		return Buffer + (sizeof(EntityHandle) + layout.Offset) * Capacity;
	}

	char* Chunk::GetComponentPtr(
		const ArchetypeLayout& layout,
		size_t index)
	{
		return GetComponentStream(layout)
			+ layout.Component->Size
			* (index - IndexOffset);
	}
}
}
