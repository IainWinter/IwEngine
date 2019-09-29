#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include "Component.h"
#include "Entity.h"
#include "EntityComponentArray.h"
#include "ComponentData.h"
#include <unordered_map>
#include <memory>

namespace IwEntity {
	class IWENTITY_API ComponentManager {
	private:
		std::unordered_map<ComponentType, iwu::ref<Component>> m_components;
		std::unordered_map<size_t, ChunkList> m_componentData;
		const size_t m_chunkSize = 16 * 1024;

	public:
		// Components

		iwu::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		iwu::ref<Component>& GetComponent(
			ComponentType type);

		// Component Data

		size_t ReserveEntityComponents(
			const iwu::ref<EntityData>& entityData);

		bool ReinstateEntityComponents(
			const iwu::ref<EntityData>& entityData);

		bool DestroyEntityComponents(
			const iwu::ref<EntityData>& entityData);

		EntityComponentArray Query(
			const iwu::ref<ArchetypeQuery>& query);
	private:
		ChunkList* FindChunkList(
			const iwu::ref<Archetype>& archetype);

		ChunkList& FindOrCreateChunkList(
			const iwu::ref<Archetype>& archetype);
	};
}
