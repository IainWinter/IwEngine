#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "Chunk.h"
#include <vector>

namespace IwEntity {
	class ChunkList {
	public:
		class iterator {
		public:
		private:
			Chunk* m_chunk;
			size_t m_index;

		public:
			iterator& operator++() {
				m_index++;

				if (m_index == m_chunk->Count) {
					if (m_chunk->Next) {
						m_chunk = m_chunk->Next;
						m_index = 0;
					}
				}

				return *this;
			}

			bool operator==(
				const iterator& itr)
			{
				return  this->m_chunk == itr.m_chunk
					&& this->m_index == itr.m_index;
			}

			bool operator!=(
				const iterator& itr)
			{
				return !operator==(itr);
			}

			Entity& operator*() {
				return *((Entity*)m_chunk->Buffer + m_index); // todo: temp
			}
		private:
			friend class ChunkList;

			iterator(
				Chunk* chunk,
				size_t index)
				: m_chunk(chunk)
				, m_index(index)
			{}
		};
	private:
		Chunk* m_current;
		std::vector<Chunk*> m_chunks;

		size_t m_count;

		iwu::ref<Archetype> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

	public:
		ChunkList(
			iwu::ref<Archetype> archetype,
			size_t chunkSize);

		void ReserveComponents(
			iwu::ref<Entity> entity);

		bool FreeComponents(
			iwu::ref<Entity> entity);

		iterator begin() {
			return iterator(m_chunks.front(), 0);
		}

		iterator end() {
			return iterator(m_chunks.back(), m_chunks.back()->Count);
		}
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk* FindOrCreateChunk();

		size_t GetChunkCapacity(
			iwu::ref<Archetype> archetype);

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
