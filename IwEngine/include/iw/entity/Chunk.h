#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;

		std::weak_ptr<Archetype2> Archetype;

		size_t Capacity;
		size_t Count;

		size_t CurrentIndex;

		char Buffer[];

		static const size_t ChunkSize;
		static const size_t HeaderSize;
		static const size_t BufferSize;

		char* GetStream(
			const ArchetypeLayout& layout);

		size_t AddEntity(
			const Entity2& entity);

		static size_t CalculateCapacity(
			std::weak_ptr<Archetype2> archetype);
	};
}
