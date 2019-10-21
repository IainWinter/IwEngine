#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Entity.h"

namespace IwEntity {
	struct IWENTITY_API Chunk {
		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t Capacity;
		size_t IndexOffset;
		size_t CurrentIndex;

		char Buffer[];

		inline size_t ReserveComponents() {
			++Count;
			return CurrentIndex++ + IndexOffset;
		}

		inline void ReinstateComponents() {
			++Count;
		}

		inline void FreeComponents() {
			--Count;
		}

		inline bool ContainsIndex(
			size_t index) const
		{
			return  IndexOffset <= index
				&& IndexOffset + CurrentIndex > index;
		}
	
		inline size_t EndIndex() {
			return CurrentIndex + IndexOffset;
		}

		size_t BeginIndex();

		Entity* GetEntity(
			size_t index);

		char* GetComponentStream(
			const ArchetypeLayout& layout);

		char* GetComponentData(
			const ArchetypeLayout& layout,
			size_t index);
	};
}
