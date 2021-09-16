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
		const size_t m_chunkSize = 16 * 1024; // chunks should double in size

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
			const EntityData& entityData);

		IWENTITY_API
		bool ReinstateComponentData(
			const EntityData& entityData);

		IWENTITY_API
		bool DestroyComponentsData(
			const EntityData& entityData);

		IWENTITY_API
		size_t MoveComponentData(
			const EntityData& entityData,
			const Archetype& archetype);

		IWENTITY_API
		void* GetComponentPtr(
			const EntityData& entityData,
			const ref<Component>& component);

		IWENTITY_API
		ref<ComponentData> GetComponentData(
			const EntityData& entityData);

		IWENTITY_API
		ComponentQuery MakeQuery(
			std::initializer_list<ref<Component>> all,
			std::initializer_list<ref<Component>> any = {},
			std::initializer_list<ref<Component>> none = {});

		IWENTITY_API
		EntityComponentArray Query(
			const ComponentQuery& components,
			const ArchetypeQuery& query);

		IWENTITY_API
		EntityHandle FindEntity(
			ArchetypeQuery& query,
			const ref<Component>& component,
			void* instance);

		IWENTITY_API
		bool SetEntityAliveState(
			const EntityData& entityData);

		IWENTITY_API
		void Clear();
	private:
		ChunkList* FindChunkList(
			const Archetype& archetype);

		ChunkList& FindOrCreateChunkList(
			const Archetype& archetype);
	};
}

	using namespace ECS;
}
