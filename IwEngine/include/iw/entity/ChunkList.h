#pragma once

#include "IwEntity.h"
#include "Chunk.h"
#include "EntityData.h"
#include "Archetype.h"
#include "Component.h"
#include "ComponentData.h"
#include "EntityComponentData.h"
#include "iw/util/memory/pool_allocator.h"
#include <vector>
#include <assert.h>

namespace iw {
namespace ECS {
	class ChunkList {
	public:
		class iterator {
		private:
			Chunk* m_chunk;
			size_t Triangles;
			iw::ref<Archetype> m_archetype;

			iw::ref<ComponentDataIndices> m_indices;
			iw::ref<ComponentData> m_data;

		public:
			IWENTITY_API
			iterator& operator++();

			IWENTITY_API
			bool operator==(
				const iterator& itr) const;

			IWENTITY_API
			bool operator!=(
				const iterator& itr) const;

			IWENTITY_API
			EntityComponentData operator*();
		private:
			friend class ChunkList;

			iterator(
				Chunk* chunk,
				size_t index,
				const iw::ref<Archetype>& archetype,
				const std::vector<iw::ref<Component>>& components,
				iw::pool_allocator& componentPool);
		};
	private:
		Chunk* m_root;

		size_t m_count;
		size_t m_chunkCount;

		iw::ref<Archetype> m_archetype;
		const size_t m_chunkSize;
		const size_t m_chunkCapacity;

		iw::pool_allocator& m_componentPool;
		iw::pool_allocator& m_chunkPool;

	public:
		IWENTITY_API
		ChunkList(
			const iw::ref<Archetype>& archetype,
			size_t chunkSize,
			iw::pool_allocator& componentPool,
			iw::pool_allocator& chunkPool);

		IWENTITY_API
		size_t ReserveComponents(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		bool ReinstateComponents(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		bool FreeComponents(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		bool MoveComponents(
			ChunkList& to,
			size_t index,
			size_t newIndex);

		IWENTITY_API
		void* GetComponentPtr(
			const iw::ref<Component>& component,
			size_t index);

		IWENTITY_API
		EntityHandle* GetEntity(
			size_t index);

		IWENTITY_API
		int IndexOf(
			const iw::ref<Component>& component,
			void* instance);

		IWENTITY_API
		iterator Begin(
			const iw::ref<ComponentQuery>& query);

		IWENTITY_API
		iterator End(
			const iw::ref<ComponentQuery>& query);
	private:
		Chunk* FindChunk(
			size_t index);
		
		Chunk* CreateChunk();

		Chunk& FindOrCreateChunk();

		size_t GetChunkCapacity(
			const iw::ref<Archetype>& archetype);

		inline bool ChunkIsFull(
			const Chunk* chunk)
		{
			return chunk->CurrentIndex == m_chunkCapacity;
		}
	};
}

	using namespace ECS;
}
