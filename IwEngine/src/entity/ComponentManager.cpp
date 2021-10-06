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

	ref<Component>& ComponentManager::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		ref<Component>& component = m_components[type];
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
			component->DeepCopyFunc = [size](void* ptr, void* data) {
				memcpy(ptr, data, size);
			};

			component->Id = m_components.size();
		}

		return component;
	}

	ref<Component> ComponentManager::GetComponent(
		ComponentType type)
	{
		auto itr = m_components.find(type);
		if (itr == m_components.end()) {
			return ref<Component>(nullptr);
		}

		return itr->second;
	}

	size_t ComponentManager::CreateComponentsData(
		const EntityData& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData.Archetype);
		return list.ReserveComponents(entityData);
	}

	bool ComponentManager::ReinstateComponentData(
		const EntityData& entityData)
	{
		ChunkList& list = FindOrCreateChunkList(entityData.Archetype);
		return list.ReinstateComponents(entityData);
	}

	bool ComponentManager::DestroyComponentData( //todo: find way to get archetype to this function
		const EntityData& entityData)
	{
		ChunkList* list = FindChunkList(entityData.Archetype);
		if (!list) {
			return false;
		}
		
		return list->FreeComponents(entityData);
	}

	size_t ComponentManager::MoveComponentData(
		const EntityData& entityData,
		const Archetype& archetype)
	{
		ChunkList* from = FindChunkList(entityData.Archetype);
		if (!from) {
			return -1;
		}
	
		ChunkList& to = FindOrCreateChunkList(archetype);
			
		size_t chunkIndex = to.ReserveComponents(entityData); // check for reinstate??

		from->MoveComponents(to, entityData.ChunkIndex, chunkIndex);
		from->FreeComponents(entityData);

		return chunkIndex;
	}

	void* ComponentManager::GetComponentPtr(
		const EntityData& entityData,
		const ref<Component>& component)
	{
		ChunkList* list = FindChunkList(entityData.Archetype);
		if (!list) {
			return nullptr;
		}
		
		return list->GetComponentPtr(component, entityData.ChunkIndex);
	}

	ref<ComponentData> ComponentManager::GetComponentData(
		const EntityData& entityData)
	{
		ChunkList* list = FindChunkList(entityData.Archetype);
		if (!list) {
			return nullptr;
		}

		size_t cdSize = sizeof(ComponentData)
					  + sizeof(size_t)
					  * entityData.Archetype.Count;

		ref<ComponentData> data = m_componentPool.alloc_ref<ComponentData>(cdSize);

		for (size_t i = 0; i < entityData.Archetype.Count; i++) 
		{
			data->Components[i] = list->GetComponentPtr(
				entityData.Archetype.Layout[i].Component, 
				entityData.ChunkIndex
			);
		}

		return data;
	}

	ComponentQuery ComponentManager::MakeQuery(
		std::initializer_list<ref<Component>> all,
		std::initializer_list<ref<Component>> any,
		std::initializer_list<ref<Component>> none)
	{
		ComponentQuery query;
		query.SetAll(all);
		query.SetAny(any);
		query.SetNone(none);

 		return query;
	}

	EntityHandle ComponentManager::FindEntity(
		ArchetypeQuery& query,
		const ref<Component>& component, 
		void* instance)
	{
		for (size_t i = 0; i < query.Count; i++) 
		{
			auto itr = m_componentData.find(query.Hashes[i]);
			if (itr == m_componentData.end()) {
				continue;
			}

			ChunkList& list = itr->second;
			
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

	bool ComponentManager::SetEntityAliveState(
		const EntityData& entityData)
	{
		ChunkList* list = FindChunkList(entityData.Archetype);
		if (!list) {
			return false;
		}

		EntityHandle* handle = list->GetEntity(entityData.ChunkIndex);
		if (!handle) {
			return false;
		}

		handle->Alive = entityData.Entity.Alive;

		return true;
	}

	void ComponentManager::Clear() {
		m_components.clear();
		m_componentData.clear();
		m_chunkPool.reset();
		m_componentPool.reset();
	}

	EntityComponentArray ComponentManager::Query(
		const ComponentQuery& components,
		const ArchetypeQuery& query)
	{
		std::vector<ChunkList::iterator> begins;
		std::vector<ChunkList::iterator> ends;
		for (size_t i = 0; i < query.Count; i++) 
		{
			auto itr = m_componentData.find(query.Hashes[i]);
			if (itr != m_componentData.end()) 
			{
				ChunkList::iterator b = itr->second.Begin(components);
				ChunkList::iterator e = itr->second.End  (components);

				if (b == e) continue;

				begins.emplace_back(std::move(b));
				ends  .emplace_back(std::move(e));
			}
		}

		return EntityComponentArray(std::move(begins), std::move(ends));
	}

	ChunkList* ComponentManager::FindChunkList(
		const Archetype& archetype)
	{
		auto itr = m_componentData.find(archetype.Hash);
		if (itr == m_componentData.end()) 
		{
			return nullptr;
		}

		return &itr->second;
	}

	ChunkList& ComponentManager::FindOrCreateChunkList(
		const Archetype& archetype)
	{
		auto itr = m_componentData.find(archetype.Hash);
		if (itr == m_componentData.end()) 
		{
			itr = m_componentData.emplace(
				archetype.Hash, 
				ChunkList(archetype, m_componentPool, m_chunkPool)
			).first;
		}

		return itr->second;
	}
}
}
