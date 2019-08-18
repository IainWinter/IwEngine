#include "iw/entity2/Chunk.h"

namespace IwEntity2 {
	size_t Chunk::EntityCapacity(
		const Archetype& archetype)
	{
		size_t archetypeSize = archetype.Size();
		size_t padding = BufferSize % archetypeSize;
		return (BufferSize - padding) / archetypeSize;
	}

	Chunk* Chunk::MakeChunk(
		const Archetype& archetype,
		Chunk* next = nullptr)
	{
		char*  buffer = (char*) malloc(ChunkSize);
		Chunk* chunk  = (Chunk*)buffer;

		chunk->Next     = next;
		chunk->Count    = 0;
		chunk->Capacity = EntityCapacity(archetype);

		return chunk;
	}
}
