#pragma once

#include "iw/entity/ComponentManager.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IwEntity {
	iwu::ref<Component>& ComponentManager::RegisterComponent(
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

	iwu::ref<Component>& ComponentManager::GetComponent(
		ComponentType type)
	{
		auto itr = m_components.find(type);
		if (itr != m_components.end()) {
			return itr->second;
		}

		return iwu::ref<Component>(nullptr);
	}

	size_t ComponentManager::ReserveEntityComponents(
		const iwu::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReserveComponents(entityData->Entity);
	}

	bool ComponentManager::ReinstateEntityComponents(
		const iwu::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReinstateComponents(entityData);
	}

	bool ComponentManager::DestroyEntityComponents( //todo: find way to get archetype to this function
		const iwu::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->FreeComponents(entityData->ChunkIndex);
		}

		return false;
	}

	void* ComponentManager::GetComponentData(
		const iwu::ref<EntityData>& entityData,
		const iwu::ref<Component>& component)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->GetComponentData(component, entityData->ChunkIndex);
		}

		return nullptr;
	}

	EntityComponentArray ComponentManager::Query(
		const iwu::ref<ComponentQuery>& components,
		const iwu::ref<ArchetypeQuery>& query)
	{
		std::vector<ChunkList::iterator>  begins;
		std::vector<ChunkList::iterator>  ends;
		for (size_t i = 0; i < query->Count; i++) {
			auto itr = m_componentData.find(query->Hashes[i]);
			if (itr != m_componentData.end()) {
				ChunkList::iterator bitr = itr->second.begin(components);
				ChunkList::iterator eitr = itr->second.end(components);
				//if (bitr != eitr) {
					begins.emplace_back(std::move(bitr));
					ends  .emplace_back(std::move(eitr));
				//}
			}
		}

		return EntityComponentArray(std::move(begins), std::move(ends)); // pass it here
	}

	ChunkList* ComponentManager::FindChunkList(
		const iwu::ref<Archetype>& archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		const iwu::ref<Archetype>& archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			itr = m_componentData.emplace(
				archetype->Hash, ChunkList(archetype, m_chunkSize)).first;
		}

		return itr->second;
	}
}
