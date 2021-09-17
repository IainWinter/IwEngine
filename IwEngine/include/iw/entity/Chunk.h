#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "EntityHandle.h"

namespace iw {
namespace ECS {
	struct Chunk {
		Chunk* Next = nullptr;
		Chunk* Previous = nullptr;

		size_t Count = 0;
		size_t Capacity = 0;
		size_t IndexOffset = 0;
		size_t CurrentIndex = 0;

		// buffer is always contiguous with header (ChunkList::CreateChunk)
		char* Buffer = nullptr;

		IWENTITY_API
		size_t ReserveComponents();

		IWENTITY_API
		void ReinstateComponents();

		IWENTITY_API
		void FreeComponents();

		IWENTITY_API
		bool ContainsIndex(
			size_t index) const;

		IWENTITY_API
		size_t EndIndex() const;

		IWENTITY_API
		size_t BeginIndex();

		IWENTITY_API
		bool IsFull() const;

		IWENTITY_API
		int IndexOf(
			const ArchetypeLayout& layout,
			void* instance) const;

		IWENTITY_API
		EntityHandle* GetEntity(
			size_t index);

		IWENTITY_API
		const EntityHandle* GetEntity(
			size_t index) const;

		IWENTITY_API
		char* GetComponentStream(
			const ArchetypeLayout& layout);

		IWENTITY_API
		const char* GetComponentStream(
			const ArchetypeLayout& layout) const;

		IWENTITY_API
		char* GetComponentPtr(
			const ArchetypeLayout& layout,
			size_t index);
	};
}

	using namespace ECS;
}
