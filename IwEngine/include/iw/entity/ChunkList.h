#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Chunk.h"
#include <vector>

namespace IwEntity {
	class ChunkList {
	private:
		Chunk* m_root;
		std::vector<Chunk*> m_chunks;

		iwu::ref<const Archetype2> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

	public:
		ChunkList(
			iwu::ref<const Archetype2> archetype,
			size_t chunkSize);

		std::shared_ptr<ComponentData> ReserveComponents(
			iwu::ref<const Entity2> entity);

		bool FreeComponents(
			iwu::ref<const Entity2> entity);
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk* FindOrCreateChunk();

		size_t GetChunkCapacity(
			iwu::ref<const Archetype2> archetype);

		char* GetChunkStream(
			Chunk* chunk,
			const ArchetypeLayout& layout);

		char* GetComponentData(
			Chunk* chunk,
			const ArchetypeLayout& layout,
			size_t index);

		inline bool ChunkIsFull(
			const Chunk* chunk)
		{
			return chunk->Count == m_chunkCapacity;
		}
	};
}
