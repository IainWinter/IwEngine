#include "iw/entity/Chunk.h"

namespace IwEntity {
	const size_t Chunk::ChunkSize  = 6 * 1024;
	const size_t Chunk::HeaderSize = 128;
	const size_t Chunk::BufferSize = ChunkSize - HeaderSize;

	char* Chunk::GetStream(
		const ArchetypeLayout& layout)
	{
		return Buffer + (layout.Offset + sizeof(Entity2)) * Capacity;
	}

	size_t Chunk::AddEntity(
		const Entity2& entity)
	{
		Entity2* ptr = (Entity2*)Buffer + CurrentIndex;
		*ptr = entity;

		Count++;
		return CurrentIndex++;
	}

	size_t Chunk::CalculateCapacity(
		std::weak_ptr<Archetype2> archetype)
	{
		size_t size = archetype.lock()->Size + sizeof(Entity2);
		return (BufferSize - BufferSize % size) / size;
	}
}
