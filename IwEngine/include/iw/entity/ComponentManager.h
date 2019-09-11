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
		std::unordered_map<ComponentType, std::shared_ptr<Component>> m_components;
		std::unordered_map<size_t, ChunkList> m_componentData;
		const size_t m_chunkSize = 16 * 1024;

	public:
		std::weak_ptr<Component> RegisterComponent(
			ComponentType type,
			size_t size);

		void ReserveComponents(
			std::weak_ptr<Entity2> entity);

		bool DestroyComponents(
			std::weak_ptr<Entity2> entity);
	private:
		ChunkList* FindChunkList(
			std::shared_ptr<Archetype2> archetype);

		ChunkList& FindOrCreateChunkList(
			std::shared_ptr<Archetype2> archetype);
	};
}
