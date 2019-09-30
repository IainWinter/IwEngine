#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t Capacity;
		size_t EntityIndex;
		size_t CurrentIndex;

		char Buffer[];

		size_t ReserveComponents();

		void ReinstateComponents();

		void FreeComponents();

		bool IsLast(
			size_t index);

		size_t LastIndex();

		Entity* GetEntity(
			size_t index);

		char* GetComponentStream(
			const ArchetypeLayout& layout);

		char* GetComponentData(
			const ArchetypeLayout& layout,
			size_t index);
	};
}
