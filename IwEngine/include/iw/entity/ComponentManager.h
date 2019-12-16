#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentData.h"
#include "ChunkList.h"
#include "EntityComponentArray.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>
#include <memory>

namespace IW {
namespace ECS {
	class ComponentManager {
	private:
		const size_t m_chunkSize = 16 * 1024;

		std::unordered_map<ComponentType, iw::ref<Component>> m_components;
		std::unordered_map<size_t, ChunkList> m_componentData;
		iw::pool_allocator m_itrPool;
		iw::pool_allocator m_chunkPool;
		iw::pool_allocator m_componentPool;

	public:
		ComponentManager();

		// Components

		iw::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		IWENTITY_API
		iw::ref<Component> GetComponent(
			ComponentType type);

		// Component Data

		size_t ReserveEntityComponents(
			const iw::ref<EntityData>& entityData);

		bool ReinstateEntityComponents(
			const iw::ref<EntityData>& entityData);

		bool DestroyEntityComponents(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		void* GetComponentData(
			const iw::ref<EntityData>& entityData,
			const iw::ref<Component>& component);

		EntityComponentArray Query(
			const iw::ref<ComponentQuery>& components,
			const iw::ref<ArchetypeQuery>& query);
	private:
		ChunkList* FindChunkList(
			const iw::ref<Archetype>& archetype);

		ChunkList& FindOrCreateChunkList(
			const iw::ref<Archetype>& archetype);
	};
}

	using namespace ECS;
}
