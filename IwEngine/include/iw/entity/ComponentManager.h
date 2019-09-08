#pragma once

#include "IwEntity.h"
#include "Component.h"
#include "ComponentData.h"
#include "Entity.h"
#include "Chunk.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace IwEntity {
	class ComponentManager {
	private:
		std::unordered_map<ComponentType, std::shared_ptr<Component>> m_components;
		std::unordered_map<size_t, Chunk*> m_componentData;

	public:
		std::weak_ptr<Component> RegisterComponent(
			ComponentType type,
			size_t size)
		{
			auto& component = m_components[type];
			if (!component) {
				component = std::make_shared<Component>();
				component->Type = type.hash_code();
				component->Name = type.name();
				component->Size = size;
			}

			return component;
		}

		void CreateComponents(
			Entity2& entity)
		{
			Archetype2& archetype = entity.Archetype.Get();
			Chunk*& chunk = m_componentData[archetype.Hash];
			if (!chunk) {
				size_t bufSize = sizeof(Chunk)
					+ sizeof(char)
					* Chunk::ChunkSize; // chunk size

				chunk = (Chunk*)malloc(bufSize);
				assert(chunk);
				memset(chunk, 0, bufSize);

				chunk->Archetype = entity.Archetype;
				chunk->Capacity  = Chunk::CalculateCapacity(entity.Archetype);
			}

			ComponentData data;
			for (size_t i = 0; i < archetype.Count; i++) {
				void* component = chunk->GetNextFree(archetype.Layout[i]);
				
			}

			chunk->Count++;
		}
	};
}
