#pragma once

#include "iw/entity/ComponentManager.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IwEntity {
	iwu::ref<Component> ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		iwu::ref<Component>& component = m_components[type];
		if (!component) {
			component = std::make_shared<Component>();
			component->Type = type.hash_code();
			component->Name = type.name();
			component->Size = size;
		}

		return component;
	}

	iwu::ref<Component> ComponentManager::GetComponent(
		ComponentType type)
	{
		auto itr = m_components.find(type);
		if (itr != m_components.end()) {
			return itr->second;
		}

		return iwu::ref<Component>();
	}

	void ComponentManager::ReserveEntityComponents(
		iwu::ref<Entity> entity,
		iwu::ref<Archetype> archetype)
	{
		ChunkList& list = FindOrCreateChunkList(archetype);
		return list.ReserveComponents(entity);
	}

	bool ComponentManager::DestroyEntityComponents( //todo: find way to get archetype to this function
		iwu::ref<Entity> entity)
	{
		//ChunkList* list = FindChunkList(entity->Archetype);
		//if (list) {
		//	return list->FreeComponents(entity);
		//}

		return false;
	}

	EntityComponentArray ComponentManager::Query(
		iwu::ref<ArchetypeQuery> query)
	{
		std::vector<ChunkList::iterator> begins;
		std::vector<ChunkList::iterator> ends;
		for (size_t i = 0; i < query->Count; i++) {
			auto itr = m_componentData.find(query->Hashes[i]);
			if (itr != m_componentData.end()) {
				begins.push_back(itr->second.begin());
				ends  .push_back(itr->second.end());
			}
		}

		return EntityComponentArray(std::move(begins), std::move(ends));
	}

	ChunkList* ComponentManager::FindChunkList(
		iwu::ref<Archetype> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		iwu::ref<Archetype> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			itr = m_componentData.emplace(archetype->Hash,
				ChunkList { 
					archetype, m_chunkSize 
				}).first;
		}

		return itr->second;
	}
}
