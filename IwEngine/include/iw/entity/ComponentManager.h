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

namespace iw {
namespace ECS {
	class ComponentManager {
	private:
		const size_t m_chunkSize = 16 * 1024;

		std::unordered_map<ComponentType, ref<Component>> m_components;
		std::unordered_map<size_t, ChunkList> m_componentData;
		pool_allocator m_chunkPool;
		pool_allocator m_componentPool;

	public:
		IWENTITY_API
		ComponentManager();

		IWENTITY_API
		ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		IWENTITY_API
		ref<Component> GetComponent(
			ComponentType type);

		IWENTITY_API
		size_t CreateComponentsData(
			const ref<EntityData>& entityData);

		IWENTITY_API
		bool ReinstateComponentData(
			const ref<EntityData>& entityData);

		IWENTITY_API
		bool DestroyComponentsData(
			const ref<EntityData>& entityData);

		IWENTITY_API
		size_t MoveComponentData(
			const ref<EntityData>& entityData,
			const ref<Archetype>& archetype);

		IWENTITY_API
		void* GetComponentPtr(
			const ref<EntityData>& entityData,
			const ref<Component>& component);

		IWENTITY_API
		ref<ComponentData> GetComponentData(
			const ref<EntityData>& entityData);

		IWENTITY_API
		ref<ComponentQuery> MakeQuery(
			std::initializer_list<ref<Component>> all,
			std::initializer_list<ref<Component>> any = {},
			std::initializer_list<ref<Component>> none = {});

		IWENTITY_API
		EntityComponentArray Query(
			const ref<ComponentQuery>& components,
			const ref<ArchetypeQuery>& query);

		IWENTITY_API
		EntityHandle FindEntity(
			ref<ArchetypeQuery>& query,
			const ref<Component>& component,
			void* instance);

		IWENTITY_API
		bool SetEntityAliveState(
			const ref<EntityData>& entityData);

		IWENTITY_API
		void Clear();
	private:
		ChunkList* FindChunkList(
			const ref<Archetype>& archetype);

		ChunkList& FindOrCreateChunkList(
			const ref<Archetype>& archetype);
	};
}

	using namespace ECS;
}
