#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include "Component.h"
#include "Entity.h"
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
		iwu::ref<Component> RegisterComponent(
			ComponentType type,
			size_t size);

		iwu::ref<ComponentData> ReserveComponents(
			iwu::ref<const Entity2> entity);

		bool DestroyComponents(
			iwu::ref<const Entity2> entity);
	private:
		ChunkList* FindChunkList(
			iwu::ref<const Archetype2> archetype);

		ChunkList& FindOrCreateChunkList(
			iwu::ref<const Archetype2> archetype);
	};
}
