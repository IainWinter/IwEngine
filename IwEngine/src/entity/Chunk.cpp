#include "iw/entity/Chunk.h"
#include <iw\log\logger.h>

namespace iw {
namespace ECS {
	size_t Chunk::ReserveComponents() {
		++Count;

		LOG_INFO << "Reserving " << Count << " of " << Capacity;

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

	bool Chunk::IsFull() const
	{
		return Count == Capacity;
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
			//if (GetEntity(index)->Alive) { // I dont think an entity should have to be alive for this to return, only if its destroied, which clears the mem
				return (int)index;
			//}
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

	void Chunk::ZeroComponentData(
		const Archetype& archetype, 
		size_t index)
	{
		for (size_t i = 0; i < archetype.Count; i++) 
		{
			const ArchetypeLayout& layout = archetype.Layout[i];

			void* component = GetComponentPtr(layout, index);
			memset(component, 0, layout.Component->Size);
		}
	}

	void Chunk::DestroyEntityComponentData(
		const Archetype& archetype, 
		size_t index)
	{
		EntityHandle* entityComponent = GetEntity(index);
		*entityComponent = EntityHandle::Empty;

		for (size_t i = 0; i < archetype.Count; i++) 
		{
			ArchetypeLayout& layout = archetype.Layout[i];
			void* component = GetComponentPtr(layout, index);;
			
			layout.Component->DestructorFunc(component);
//#ifdef IW_DEBUG
			memset(component, 0/*0xef*/, layout.Component->Size); // debug mem 0xef (engine free)
//#endif

			// bugs without zeroing this... means issue with game !!@!
		}
	}
}
}
