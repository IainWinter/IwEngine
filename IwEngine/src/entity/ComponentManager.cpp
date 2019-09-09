#pragma once

#include "iw/entity/ComponentManager.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IwEntity {
	std::weak_ptr<Component> ComponentManager::RegisterComponent(
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

	std::shared_ptr<ComponentData> ComponentManager::CreateComponents(
		Entity2& entity)
	{
		std::shared_ptr<Archetype2> archetype = entity.Archetype.lock();
		Chunk*& chunk = m_componentData[archetype->Hash];
		if (!chunk) {
			size_t bufSize = sizeof(Chunk)
				+ sizeof(char)
				* Chunk::ChunkSize; // chunk size

			chunk = (Chunk*)malloc(bufSize);
			assert(chunk);
			memset(chunk, 0, bufSize);

			chunk->Archetype = entity.Archetype;
			chunk->Capacity = Chunk::CalculateCapacity(entity.Archetype);
		}

		size_t bufSize = sizeof(ComponentData)
			+ sizeof(void*)
			* archetype->Count;

		ComponentData* buf = (ComponentData*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		std::shared_ptr<ComponentData> data = std::shared_ptr<ComponentData>(buf);
		data->Archetype = entity.Archetype;

		size_t chunkIndex = chunk->AddEntity(entity);

		for (size_t i = 0; i < archetype->Count; i++) {
			char* stream = chunk->GetStream(archetype->Layout[i])
				+ archetype->Layout[i].Component.lock()->Size
				* chunkIndex;

			data->Components[i] = stream;
		}

		return data;
	}

	bool ComponentManager::DestroyComponents(
		Entity2& entity)
	{
		return false;
	}
}
