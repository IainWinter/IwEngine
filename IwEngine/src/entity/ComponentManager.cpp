#pragma once

#include "iw/entity/ComponentManager.h"
#include "iw/log/logger.h"
#include <unordered_map>
#include <typeindex>
#include <assert.h>
#include <memory>

namespace iw {
namespace ECS {
	ComponentManager::ComponentManager()
		: m_chunkPool(m_chunkSize)
		, m_componentPool(1024)
	{}

	iw::ref<Component>& ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		iw::ref<Component>& component = m_components[type];
		if (!component) {
			component = m_componentPool.alloc_ref<Component>();

#ifdef IW_USE_REFLECTION
			component->Type = type;
			component->Name = type->name;
			component->Size = type->size;
#else
			component->Type = type.hash_code();
			component->Name = type.name();
			component->Size = size;
#endif
		}

		return component;
	}

	iw::ref<Component> ComponentManager::GetComponent(
		ComponentType type)
	{
		auto itr = m_components.find(type);
		if (itr == m_components.end()) {
			return iw::ref<Component>(nullptr);
		}

		return itr->second;
	}

	size_t ComponentManager::CreateComponentsData(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData->Archetype);
		return list.ReserveComponents(entityData);
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
		if (!list) {
			return false;
		}
		
		return list->FreeComponents(entityData);
	}

	size_t ComponentManager::MoveComponentData(
		const iw::ref<EntityData>& entityData,
		const iw::ref<Archetype>& archetype)
	{
		ChunkList* from = FindChunkList(entityData->Archetype);
		if (!from) {
			return -1;
		}
	
		ChunkList& to = FindOrCreateChunkList(archetype);
			
		size_t chunkIndex = to.ReserveComponents(entityData); // check for reinstate??

		from->MoveComponents(to, entityData->ChunkIndex, chunkIndex);
		from->FreeComponents(entityData);

		return chunkIndex;
	}

	void* ComponentManager::GetComponentPtr(
		const iw::ref<EntityData>& entityData,
		const iw::ref<Component>& component)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (!list) {
			return nullptr;
		}
		
		return list->GetComponentPtr(component, entityData->ChunkIndex);
	}

	iw::ref<ComponentData> ComponentManager::GetComponentData(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (!list) {
			return nullptr;
		}

		size_t cdSize = sizeof(ComponentData)
					  + sizeof(size_t)
					  * entityData->Archetype->Count;

		iw::ref<ComponentData> data = m_componentPool.alloc_ref<ComponentData>(cdSize);

		for (size_t i = 0; i < entityData->Archetype->Count; i++) {
			data->Components[i] = list->GetComponentPtr(
				entityData->Archetype->Layout[i].Component, 
				entityData->ChunkIndex
			);
		}

		return data;
	}

	EntityHandle ComponentManager::FindEntity(
		iw::ref<ArchetypeQuery>& query,
		const iw::ref<Component>& component, 
		void* instance)
	{
		for (size_t i = 0; i < query->Count; i++) {
			ChunkList& list = m_componentData.find(query->Hashes[i])->second;
			
			int index = list.IndexOf(component, instance);
			if (index == -1) {
				continue;
			}

			EntityHandle* handle = list.GetEntity(index);
			if (handle) {
				return *handle;
			}

			break;
		}

		return EntityHandle::Empty;
	}

	iw::ref<ComponentQuery> ComponentManager::MakeQuery(
		std::initializer_list<iw::ref<Component>> components)
	{
		size_t bufSize = sizeof(ComponentQuery)
			+ sizeof(iw::ref<Component>)
			* components.size();

		iw::ref<ComponentQuery> query = m_componentPool.alloc_ref<ComponentQuery>(bufSize);

		query->Count = components.size();

		size_t i = 0;
		for (iw::ref<Component> component : components) {
			query->Components[i++] = component;
		}

		return query;
	}

	bool ComponentManager::SetEntityAliveState(
		const iw::ref<EntityData>& entityData)
	{
		ChunkList* list = FindChunkList(entityData->Archetype);
		if (!list) {
			return false;
		}

		EntityHandle* handle = list->GetEntity(entityData->ChunkIndex);
		if (!handle) {
			return false;
		}

		handle->Alive = entityData->Entity.Alive;

		return true;
	}

	void ComponentManager::Clear() {
		m_components.clear();
		m_componentData.clear();
		m_chunkPool.reset();
		m_componentPool.reset();
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
				ChunkList::iterator b = itr->second.Begin(components);
				ChunkList::iterator e = itr->second.End  (components);

				if (b == e) continue;

				begins.emplace_back(std::move(b));
				ends  .emplace_back(std::move(e));
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
}
