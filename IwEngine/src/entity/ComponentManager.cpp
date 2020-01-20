#pragma once

#include "iw/entity/ComponentManager.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace IW {
	ComponentManager::ComponentManager()
		: m_chunkPool(m_chunkSize)
		, m_componentPool(512)
	{}

	iw::ref<Component>& ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		iw::ref<Component>& component = m_components[type];
		if (!component) {
			component = m_componentPool.alloc_ref_t<Component>();
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

	size_t ComponentManager::CreateComponentsData(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReserveComponents(entityData->Entity);
	}

	bool ComponentManager::ReinstateComponentData(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReinstateComponents(entityData);
	}

	bool ComponentManager::DestroyComponentsData( //todo: find way to get archetype to this function
		const iw::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->FreeComponents(entityData->ChunkIndex);
		}

		return false;
	}

	size_t ComponentManager::MoveComponentData(
		const iw::ref<EntityData>& entityData,
		const iw::ref<Archetype>& archetype)
	{
		ChunkList* from = FindChunkList(entityData->Archetype);
		if (from) {
			ChunkList& to = FindOrCreateChunkList(archetype);
			
			size_t chunkIndex = to.ReserveComponents(entityData->Entity); // check for reinstate??

			from->MoveComponents(to, entityData->ChunkIndex, chunkIndex);
			from->FreeComponents(entityData->ChunkIndex);

			return chunkIndex;
		}

		return -1;
	}

	void* ComponentManager::GetComponentPtr(
		const iw::ref<EntityData>& entityData,
		const iw::ref<Component>& component)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			return list->GetComponentPtr(component, entityData->ChunkIndex);
		}

		return nullptr;
	}

	iw::ref<ComponentData> ComponentManager::GetComponentData(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (list) {
			size_t cdSize = sizeof(ComponentData)
				+ sizeof(size_t)
				* entityData->Archetype->Count;

			iw::ref<ComponentData> data = m_componentPool.alloc_ref_t<ComponentData>(cdSize);

			for (size_t i = 0; i < entityData->Archetype->Count; i++) {
				data->Components[i] = list->GetComponentPtr(
					entityData->Archetype->Layout[i].Component, entityData->ChunkIndex);
			}

			return data;
		}

		return nullptr;
	}

	EntityHandle ComponentManager::FindEntity(
		iw::ref<ArchetypeQuery>& query,
		const iw::ref<Component>& component, 
		void* instance)
	{
		for (size_t i = 0; i < query->Count; i++) {
			ChunkList& list = m_componentData.find(query->Hashes[i])->second;
			int index = list.IndexOf(component, instance);
			if (index != -1) {
				return *list.GetEntity(index);
			}
		}

		return EntityHandle::Empty;
	}

	iw::ref<ComponentQuery> ComponentManager::MakeQuery(
		std::initializer_list<iw::ref<Component>> components)
	{
		size_t bufSize = sizeof(ComponentQuery)
			+ sizeof(iw::ref<Component>)
			* components.size();

		iw::ref<ComponentQuery> query = m_componentPool.alloc_ref_t<ComponentQuery>(bufSize);

		query->Count = components.size();

		size_t i = 0;
		for (iw::ref<Component> component : components) {
			query->Components[i++] = component;
		}

		return query;
	}

	EntityComponentArray ComponentManager::Query(
		const iw::ref<ComponentQuery>& components,
		const iw::ref<ArchetypeQuery>& query)
	{
		std::vector<ChunkList::iterator> begins;
		std::vector<ChunkList::iterator> ends;
		for (size_t i = 0; i < query->Count; i++) {
			auto itr = m_componentData.find(query->Hashes[i]);
			if (itr != m_componentData.end()) {
				begins.emplace_back(std::move(itr->second.Begin(components)));
				ends  .emplace_back(std::move(itr->second.End  (components)));
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
				archetype->Hash, ChunkList(archetype, m_chunkSize, m_componentPool, m_chunkPool)).first;
		}

		return itr->second;
	}
}
