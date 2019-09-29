#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "EntityData.h"
#include "Chunk.h"
#include <vector>

namespace IwEntity {
	class ChunkList {
	public:
		class iterator { // move code to cpp
		private:
			Chunk* m_chunk;
			size_t m_index;

		public:
			iterator& operator++() {
				m_index++;
				while (!this->operator*().Alive && m_index != m_chunk->CurrentIndex) { // replace with free list like thing but for valid entities
					m_index++;
				}

				if (m_index == m_chunk->CurrentIndex) {
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
			const iwu::ref<Archetype>& archetype,
			size_t chunkSize);

		size_t ReserveComponents(
			const Entity& entity);

		bool ReinstateComponents(
			const iwu::ref<EntityData>& entityData);

		bool FreeComponents(
			size_t index);

		iterator begin();

		iterator end();
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk& FindOrCreateChunk();

		size_t GetChunkCapacity(
			const iwu::ref<Archetype>& archetype);

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
