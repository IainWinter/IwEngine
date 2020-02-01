#pragma once

#include "iw/entity/Space.h"
#include "iw/entity/Entity.h"
#include <assert.h>

namespace IW {
	iw::ref<Component>& Space::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	iw::ref<Component> Space::GetComponent(
		ComponentType type)
	{
		return m_componentManager.GetComponent(type);
	}

	iw::ref<Archetype>& Space::CreateArchetype(
		std::initializer_list<iw::ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	Entity Space::CreateEntity(
		const iw::ref<Archetype>& archetype)
	{
		iw::ref<EntityData>& entityData = m_entityManager.CreateEntity();

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
		return m_entityManager   .DestroyEntity(index)
			&& m_componentManager.DestroyComponentsData(m_entityManager.GetEntityData(index));
	}

	void Space::AddComponent(
		const EntityHandle& entity,
		const iw::ref<Component>& component)
	{
		iw::ref<EntityData>& entityData = m_entityManager   .GetEntityData(entity.Index);
		iw::ref<Archetype>   archetype  = m_archetypeManager.AddComponent (entityData->Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void Space::RemoveComponent(
		const EntityHandle& entity,
		const iw::ref<Component>& component)
	{
		iw::ref<EntityData>& entityData = m_entityManager   .GetEntityData  (entity.Index);
		iw::ref<Archetype>   archetype  = m_archetypeManager.RemoveComponent(entityData->Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void* Space::SetComponent(
		const EntityHandle& entity,
		const iw::ref<Component>& component,
		void* data)
	{
		if (component) {
			iw::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
			void* ptr = m_componentManager.GetComponentPtr(entityData, component);
			if (ptr) {
				memcpy(ptr, data, component->Size); // should pass this all the way down to Chunk.h but ugh
				return ptr;
			}
		}

		return nullptr;
	}

	void* Space::FindComponent(
		const EntityHandle& entity,
		const iw::ref<Component>& component)
	{
		if (component) {
			iw::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
			return m_componentManager.GetComponentPtr(entityData, component);
		}

		return nullptr;
	}

	bool Space::HasComponent(
		const EntityHandle& entity,
		const iw::ref<Component>& component)
	{
		if (component) {
			iw::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
			return entityData->Archetype->HasComponent(component);
		}

		return false;
	}

	iw::ref<ComponentQuery> Space::MakeQuery(
		std::initializer_list<iw::ref<Component>> components)
	{
		return m_componentManager.MakeQuery(components);
	}

	EntityComponentArray Space::Query(
		const iw::ref<ComponentQuery>& query)
	{
		return m_componentManager.Query(query, m_archetypeManager.MakeQuery(query));
	}

	Entity Space::FindEntity(
		const iw::ref<Component>& component,
		void* instance)
	{
		EntityHandle entity = m_componentManager.FindEntity(
			m_archetypeManager.MakeQuery(MakeQuery({ component })),
			component,
			instance);

		return Entity(entity , this);
	}

	void Space::MoveComponents(
		iw::ref<EntityData>& entityData,
		const iw::ref<Archetype>& archetype)
	{
		entityData->ChunkIndex = m_componentManager.MoveComponentData(entityData, archetype);
		entityData->Archetype = archetype;
	}
}
