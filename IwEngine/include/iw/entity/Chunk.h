#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "EntityHandle.h"

namespace iw {
namespace ECS {
	struct Chunk {
		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t Capacity;
		size_t IndexOffset;
		size_t CurrentIndex;

		char Buffer[];

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
