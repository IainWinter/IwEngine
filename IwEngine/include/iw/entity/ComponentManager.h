#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include "Component.h"
#include "Entity.h"
#include "EntityComponentArray.h"
#include "ComponentData.h"
#include <unordered_map>
#include <memory>

namespace IW {
namespace ECS {
	class ComponentManager {
	private:
		std::unordered_map<ComponentType, iw::ref<Component>> m_components;
		std::unordered_map<size_t, ChunkList> m_componentData;
		const size_t m_chunkSize = 16 * 1024;

	public:
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
