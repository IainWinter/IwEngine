#pragma once

#include "IwEntity.h"
#include "Chunk.h"
#include "EntityData.h"
#include "Archetype.h"
#include "Component.h"
#include "ComponentData.h"
#include "EntityComponentData.h"
#include <vector>
#include <assert.h>

namespace IwEntity {
	class IWENTITY_API ChunkList {
	public:
		class IWENTITY_API iterator {
		private:
			Chunk* m_chunk;
			size_t m_index;
			iwu::ref<Archetype> m_archetype;
			iwu::ref<ComponentDataIndices> m_indices;
			iwu::ref<ComponentData> m_data;

		public:
			iterator& operator++();

			bool operator==(
				const iterator& itr);

			bool operator!=(
				const iterator& itr);

			EntityComponentData operator*();
		private:
			friend class ChunkList;

			iterator(
				Chunk* chunk,
				size_t index,
				const iwu::ref<Archetype>& archetype,
				const iwu::ref<ComponentQuery>& query);
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

		iterator begin(
			const iwu::ref<ComponentQuery>& query);

		iterator end(
			const iwu::ref<ComponentQuery>& query);
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
