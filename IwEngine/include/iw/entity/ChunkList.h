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

			Chunk::EntityComponentType& operator*() {
				return *m_chunk->GetEntity(m_index);
			}
		private:
			friend class ChunkList;

			iterator(
				Chunk* chunk)
				: m_chunk(chunk)
				, m_index(0)
			{}
		};
	private:
		Chunk* m_current;
		std::vector<Chunk*> m_chunks;

		size_t m_count;

		iwu::ref<const Archetype2> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

	public:
		ChunkList(
			iwu::ref<const Archetype2> archetype,
			size_t chunkSize);

		std::shared_ptr<ComponentData> ReserveComponents(
			iwu::ref<Entity2> entity);

		bool FreeComponents(
			iwu::ref<const Entity2> entity);

		iterator begin() {
			return iterator(m_chunks.front());
		}

		iterator end() {
			return iterator(m_chunks.back());
		}
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
