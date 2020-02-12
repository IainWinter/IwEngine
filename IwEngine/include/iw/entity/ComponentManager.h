#pragma once

#include "IwEntity.h"
#include "EntityHandle.h"
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
		iw::pool_allocator m_chunkPool;
		iw::pool_allocator m_componentPool;

	public:
		IWENTITY_API
		ComponentManager();

		IWENTITY_API
		iw::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		IWENTITY_API
		iw::ref<Component> GetComponent(
			ComponentType type);

		IWENTITY_API
		size_t CreateComponentsData(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		bool ReinstateComponentData(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		bool DestroyComponentsData(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		size_t MoveComponentData(
			const iw::ref<EntityData>& entityData,
			const iw::ref<Archetype>& archetype);

		IWENTITY_API
		void* GetComponentPtr(
			const iw::ref<EntityData>& entityData,
			const iw::ref<Component>& component);

		IWENTITY_API
		iw::ref<ComponentData> GetComponentData(
			const iw::ref<EntityData>& entityData);

		IWENTITY_API
		iw::ref<ComponentQuery> MakeQuery(
			std::initializer_list<iw::ref<Component>> components);

		IWENTITY_API
		EntityComponentArray Query(
			const iw::ref<ComponentQuery>& components,
			const iw::ref<ArchetypeQuery>& query);

		IWENTITY_API
		EntityHandle FindEntity(
			iw::ref<ArchetypeQuery>& query,
			const iw::ref<Component>& component,
			void* instance);

		IWENTITY_API
		void Clear();
	private:
		ChunkList* FindChunkList(
			const iw::ref<Archetype>& archetype);

		ChunkList& FindOrCreateChunkList(
			const iw::ref<Archetype>& archetype);
	};
}

	using namespace ECS;
}
