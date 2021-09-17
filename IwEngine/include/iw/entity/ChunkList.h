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
			size_t Index;
			Archetype m_archetype;

			ComponentDataIndices m_indices;
			ComponentData m_data;

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
				const Archetype& archetype,
				const std::vector<ref<Component>>& components,
				pool_allocator& componentPool);
		};
	private:
		Chunk* m_root;

		size_t m_count;
		size_t m_chunkCount;

		Archetype m_archetype;

		pool_allocator& m_componentPool;
		pool_allocator& m_chunkPool;

	public:
		IWENTITY_API
		ChunkList(
			const Archetype& archetype,
			pool_allocator& componentPool,
			pool_allocator& chunkPool);

		IWENTITY_API
		size_t ReserveComponents(
			const EntityData& entityData);

		IWENTITY_API
		bool ReinstateComponents(
			const EntityData& entityData);

		IWENTITY_API
		bool FreeComponents(
			const EntityData& entityData);

		IWENTITY_API
		bool MoveComponents(
			ChunkList& to,
			size_t index,
			size_t newIndex);

		IWENTITY_API
		void* GetComponentPtr(
			const ref<Component>& component,
			size_t index);

		IWENTITY_API
		EntityHandle* GetEntity(
			size_t index);

		IWENTITY_API
		int IndexOf(
			const ref<Component>& component,
			void* instance);

		IWENTITY_API
		iterator Begin(
			const ComponentQuery& query);

		IWENTITY_API
		iterator End(
			const ComponentQuery& query);
	private:
		Chunk* FindChunk(
			size_t index);

		Chunk* CreateChunk(
			Chunk* previous);

		Chunk& FindOrCreateChunk();

		size_t GetChunkBufferSize(
			const Archetype& archetype,
			size_t capasity);

		void LogChunkInfo(
			const Chunk* chunk,
			const std::string& thing)
		{
			//std::stringstream ss;
			//ss << '<';
			//for (size_t i = 0; i < m_archetype.Count; i++)
			//{
			//	ss << m_archetype.Layout[i].Component->Name;

			//	if (i != m_archetype.Count - 1)
			//	{
			//		ss << '\n';
			//	}
			//}
			//ss << '>';

#ifdef IW_DEBUG
			LOG_DEBUG << "[ECS] " << thing << " Chunk " << chunk->IndexOffset / chunk->Capacity
				<< " (" << chunk->Capacity * m_archetype.Size << " bytes)"
				/*<< "\n" << ss.str()*/;
#endif
		}
	};
}

	using namespace ECS;
}
