#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Chunk.h"
#include <vector>

namespace IwEntity {
	class ChunkList {
	private:
		std::vector<Chunk*> m_chunks;
		Chunk* m_root;

		const std::weak_ptr<Archetype2> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

	public:
		ChunkList(
			const std::weak_ptr<Archetype2> archetype,
			size_t chunkSize);

		void ReserveComponents(
			std::shared_ptr<Entity2> entity);

		bool FreeComponents(
			std::shared_ptr<Entity2> entity);
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk* FindOrCreateChunk();

		size_t GetChunkCapacity(
			std::weak_ptr<Archetype2> archetype);

		char* GetChunkStream(
			Chunk* chunk,
			const ArchetypeLayout& layout);

		inline bool ChunkIsFull(
			const Chunk* chunk)
		{
			return chunk->Count == m_chunkCapacity;
		}
	};
}
