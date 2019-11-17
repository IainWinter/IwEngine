#pragma once

#include "iw/entity/ComponentManager.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IW {
	iw::ref<Component>& ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		iw::ref<Component>& component = m_components[type];
		if (!component) {
			component = std::make_shared<Component>();
			component->Type = type.hash_code();
			component->Name = type.name();
			component->Size = size;
		}

		return component;
	}

	iw::ref<Component> ComponentManager::GetComponent(
		ComponentType type)
	{
		auto itr = m_components.find(type);
		if (itr != m_components.end()) {
			return itr->second;
		}

		return iw::ref<Component>(nullptr);
	}

	size_t ComponentManager::ReserveEntityComponents(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReserveComponents(entityData->Entity);
	}

	bool ComponentManager::ReinstateEntityComponents(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReinstateComponents(entityData);
	}

	bool ComponentManager::DestroyEntityComponents( //todo: find way to get archetype to this function
		const iw::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->FreeComponents(entityData->ChunkIndex);
		}

		return false;
	}

	void* ComponentManager::GetComponentData(
		const iw::ref<EntityData>& entityData,
		const iw::ref<Component>& component)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->GetComponentData(component, entityData->ChunkIndex);
		}

		return nullptr;
	}

	EntityComponentArray ComponentManager::Query(
		const iw::ref<ComponentQuery>& components,
		const iw::ref<ArchetypeQuery>& query)
	{
		std::vector<ChunkList::iterator>  begins;
		std::vector<ChunkList::iterator>  ends;
		for (size_t i = 0; i < query->Count; i++) {
			auto itr = m_componentData.find(query->Hashes[i]);
			if (itr != m_componentData.end()) {
				ChunkList::iterator bitr = itr->second.Begin(components);
				ChunkList::iterator eitr = itr->second.End(components);
				//if (bitr != eitr) {
					begins.emplace_back(std::move(bitr));
					ends  .emplace_back(std::move(eitr));
				//}
			}
		}

		return EntityComponentArray(std::move(begins), std::move(ends)); // pass it here
	}

	ChunkList* ComponentManager::FindChunkList(
		const iw::ref<Archetype>& archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		const iw::ref<Archetype>& archetype)
	{
		auto itr = m_componentData.find(archetype->Hash);
		if (itr == m_componentData.end()) {
			itr = m_componentData.emplace(
				archetype->Hash, ChunkList(archetype, m_chunkSize)).first;
		}

		return itr->second;
	}
}
