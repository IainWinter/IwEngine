#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;

		EntityArchetype Archetype;

		size_t Capacity;
		size_t Count;

		size_t NextFree;

		char Buffer[];

		static const size_t ChunkSize  = 6 * 1024;
		static const size_t HeaderSize = 128;
		static const size_t BufferSize = ChunkSize - HeaderSize;

		static size_t CalculateCapacity(
			EntityArchetype archetype)
		{
			size_t size = archetype.Get().Size + sizeof(Entity);
			return (BufferSize - BufferSize % size) / size;
		}

		void* GetStream(
			const ArchetypeLayout& layout)
		{
			return Buffer + (layout.Offset + sizeof(Entity)) * Capacity;
		}

		void* GetNextFree(
			const ArchetypeLayout& layout)
		{
			return Buffer + (layout.Offset + sizeof(Entity)) * Capacity
				+ NextFree * layout.Component.lock()->Size;
		}
	};
}
