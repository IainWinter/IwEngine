#pragma once

#include "iw/entity/Space.h"
#include "iw/entity/Entity.h"
#include <assert.h>

#ifdef IW_USE_EVENTS
#	include "iw/entity/Events/EntityEvents.h"
#endif

namespace iw {
namespace ECS {
	ref<Component>& Space::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	ref<Component> Space::GetComponent(
		ComponentType type)
	{
		return m_componentManager.GetComponent(type);
	}

	ref<Archetype>& Space::CreateArchetype(
		std::initializer_list<ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	Entity Space::CreateEntity(
		const ref<Archetype>& archetype)
	{
		ref<EntityData>& entityData = m_entityManager.CreateEntity();

		bool componentsExist = false;
		if (   entityData->Archetype
			&& entityData->Archetype->Hash == archetype->Hash)
		{
			componentsExist = m_componentManager.ReinstateComponentData(entityData); // combine this logic into cmgr.CreateComponentData
		}

		else {
			entityData->Archetype = archetype;
		}

		if (!componentsExist) {
			entityData->ChunkIndex = m_componentManager.CreateComponentsData(entityData);
		}

		return Entity(entityData->Entity, this);
	}

	bool Space::DestroyEntity(
		size_t index)
	{
		if (!m_entityManager.IsValidEntityIndex(index)) {
			LOG_WARNING << "Tried to delete entity with an invalid index " << index;
			return false;
		}

		ref<EntityData>& entityData = m_entityManager.GetEntityData(index);

#ifdef IW_USE_EVENTS
		if (m_bus) {
			m_bus->send<EntityDestroyedEvent>(Entity(entityData->Entity, this));
		}
#endif

		return m_entityManager   .DestroyEntity(index)
			&& m_componentManager.DestroyComponentsData(entityData);
	}

	void Space::AddComponent(
		const EntityHandle& entity,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(entity.Index)) {
			LOG_WARNING << "Tried to add a " << component->Name
				        << " component to an entity with an invalid index " << entity.Index;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to add empty component from entity " << entity.Index;
			return;
		}

		ref<EntityData>& entityData = m_entityManager   .GetEntityData(entity.Index);
		ref<Archetype>   archetype  = m_archetypeManager.AddComponent (entityData->Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void Space::RemoveComponent(
		const EntityHandle& entity,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(entity.Index)) {
			LOG_WARNING << "Tried to remove a " << component->Name
				        << " component from an entity with an invalid index " << entity.Index;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to remove empty component from entity " << entity.Index;
			return;
		}

		ref<EntityData>& entityData = m_entityManager   .GetEntityData  (entity.Index);
		ref<Archetype>   archetype  = m_archetypeManager.RemoveComponent(entityData->Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void* Space::SetComponent(
		const EntityHandle& entity,
		const ref<Component>& component,
		void* data)
	{
		if (!m_entityManager.IsValidEntityIndex(entity.Index)) {
			LOG_WARNING << "Tried to set a " << component->Name
				        << " component's data on an entity with an invalid index " << entity.Index;
			return nullptr;
		}

		if (!component) {
			LOG_WARNING << "Tried to set empty component on entity " << entity.Index;
			return nullptr;
		}

		ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
		void* ptr = m_componentManager.GetComponentPtr(entityData, component);
		if (ptr) {
			memcpy(ptr, data, component->Size); // should pass this all the way down to Chunk.h but ugh
			return ptr;
		}
	}

	void* Space::FindComponent(
		const EntityHandle& entity,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(entity.Index)) {
			LOG_WARNING << "Tried to find a " << component->Name
				        << " component on an entity with an invalid index " << entity.Index;
			return nullptr;
		}

		if (!component) {
			LOG_WARNING << "Tried to find empty component on entity " << entity.Index;
			return nullptr;
		}

		ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
		return m_componentManager.GetComponentPtr(entityData, component);
	}

	bool Space::HasComponent(
		const EntityHandle& entity,
		const ref<Component>& component)
	{
		if (!component) {
			LOG_WARNING << "Tried to find empty component on entity " << entity.Index;
			return false;
		}

		ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
		return entityData->Archetype->HasComponent(component);
	}

	ref<ComponentQuery> Space::MakeQuery(
		std::initializer_list<ref<Component>> components)
	{
		return m_componentManager.MakeQuery(components);
	}

	EntityComponentArray Space::Query(
		const ref<ComponentQuery>& query)
	{
		return m_componentManager.Query(query, m_archetypeManager.MakeQuery(query));
	}

	Entity Space::FindEntity(
		const ref<Component>& component,
		void* instance)
	{
		if (!component) {
			LOG_WARNING << "Tried to find entity with an empty component";
			return Entity();
		}

		EntityHandle entity = m_componentManager.FindEntity(
			m_archetypeManager.MakeQuery(MakeQuery({ component })),
			component,
			instance);

		return Entity(entity, this);
	}

	IWENTITY_API
	Entity Space::Instantiate(
		const Prefab& prefab)
	{
		Entity entity = CreateEntity<>();

		auto components  = prefab.Components();
		auto compDataItr = prefab.ComponentData().begin();

		for (iw::ref<Component> component : components) {
			AddComponent(entity.Handle, component);
			
			iw::ref<EntityData>& data = m_entityManager.GetEntityData(entity.Index());
			void* compData = m_componentManager.GetComponentPtr(data, component);

			memcpy(compData, *compDataItr, component->Size);

			compDataItr++;
		}

		return entity;
	}

	void Space::Clear() {
#ifdef IW_USE_EVENTS
		if (m_bus) {
			for (const ref<EntityData>& data : m_entityManager.Entities()) {
				if (data->Entity.Alive) {
					m_bus->send<EntityDestroyedEvent>(Entity(data->Entity, this));
				}
			}
		}
#endif

		m_entityManager.Clear();
		m_componentManager.Clear();
		m_archetypeManager.Clear();
	}

	void Space::MoveComponents(
		ref<EntityData>& entityData,
		const ref<Archetype>& archetype)
	{
		entityData->ChunkIndex = m_componentManager.MoveComponentData(entityData, archetype);
		entityData->Archetype = archetype;
	}
}
}
