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
	class ChunkList {
	public:
		class iterator {
		private:
			Chunk* m_chunk;
			size_t m_index;
			iwu::ref<Archetype> m_archetype;
			iwu::ref<ComponentDataIndices> m_indices;

			iwu::ref<ComponentData> m_data;

		public:
			IWENTITY_API iterator& operator++();

			IWENTITY_API bool operator==(
				const iterator& itr) const;

			IWENTITY_API bool operator!=(
				const iterator& itr) const;

			IWENTITY_API EntityComponentData operator*();
		private:
			friend class ChunkList;

			iterator(
				Chunk* chunk,
				size_t index,
				const iwu::ref<Archetype>& archetype,
				const iwu::ref<ComponentQuery>& query);
		};
	private:
		Chunk* m_root;

		size_t m_count;
		size_t m_chunkCount;

		iwu::ref<Archetype> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

	public:
		IWENTITY_API ChunkList(
			const iwu::ref<Archetype>& archetype,
			size_t chunkSize);

		IWENTITY_API size_t ReserveComponents(
			const Entity& entity);

		IWENTITY_API bool ReinstateComponents(
			const iwu::ref<EntityData>& entityData);

		IWENTITY_API bool FreeComponents(
			size_t index);

		IWENTITY_API void* GetComponentData(
			const iwu::ref<Component>& component,
			size_t index);

		IWENTITY_API iterator Begin(
			const iwu::ref<ComponentQuery>& query);

		IWENTITY_API iterator End(
			const iwu::ref<ComponentQuery>& query);
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk& FindOrCreateChunk();

		size_t GetChunkCapacity(
			const iwu::ref<Archetype>& archetype);

		inline bool ChunkIsFull(
			const Chunk* chunk)
		{
			return chunk->CurrentIndex == m_chunkCapacity;
		}
	};
}
