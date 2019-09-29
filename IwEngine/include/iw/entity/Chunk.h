#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t EntityIndex;
		size_t CurrentIndex;

		char Buffer[];

		size_t ReserveComponents();

		void ReinstateComponents();

		void FreeComponents();

		Entity* GetEntity(
			size_t index);

		char* GetComponentStream(
			size_t capacity,
			const ArchetypeLayout& layout);
	};
}
