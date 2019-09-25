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
		std::type_index index)
	{
		auto itr = m_components.find(index);
		if (itr != m_components.end()) {
			return itr->second;
		}

		return iwu::ref<Component>();
	}

	iwu::ref<ComponentData> ComponentManager::ReserveComponents(
		iwu::ref<Entity2> entity)
	{
		ChunkList& list = FindOrCreateChunkList(entity->Archetype);
		return list.ReserveComponents(entity);
	}

	bool ComponentManager::DestroyComponents(
		iwu::ref<const Entity2> entity)
	{
		ChunkList* list = FindChunkList(entity->Archetype);
		if (list) {
			return list->FreeComponents(entity);
		}

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
		iwu::ref<const Archetype2> archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		iwu::ref<const Archetype2> archetype)
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
