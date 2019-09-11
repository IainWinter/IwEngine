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

	void ComponentManager::ReserveComponents(
		std::weak_ptr<Entity2> entity_)
	{
		std::shared_ptr<Entity2>    entity    = entity_.lock();
		std::shared_ptr<Archetype2> archetype = entity->Archetype.lock();

		ChunkList& list = FindOrCreateChunkList(archetype);
		list.ReserveComponents(entity);

		/*size_t bufSize = sizeof(ComponentData)
			+ sizeof(void*)
			* archetype->Count;

		ComponentData* buf = (ComponentData*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		entity->ComponentData = std::shared_ptr<ComponentData>(buf);
		entity->ChunkIndex    = chunk->ReserveEntity(entity);

		for (size_t i = 0; i < archetype->Count; i++) {
			char* stream = chunk->GetStream(archetype->Layout[i])
				+ archetype->Layout[i].Component.lock()->Size
				* entity->ChunkIndex;

			entity->ComponentData->Components[i] = stream;
		}*/
	}

	bool ComponentManager::DestroyComponents(
		std::weak_ptr<Entity2> entity_)
	{
		std::shared_ptr<Entity2>    entity    = entity_.lock();
		std::shared_ptr<Archetype2> archetype = entity->Archetype.lock();

		ChunkList* list = FindChunkList(archetype);
		if (list) {
			return list->FreeComponents(entity);
		}

		return false;
	}

	ChunkList* ComponentManager::FindChunkList(
		std::shared_ptr<Archetype2> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		std::shared_ptr<Archetype2> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			itr = m_componentData.emplace(archetype->Hash, ChunkList { archetype, m_chunkSize }).first;
		}

		return itr->second;
	}
}
