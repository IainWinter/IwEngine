#pragma once

#include "iw/entity/Space.h"

namespace IwEntity {
	iwu::ref<Component>& Space::RegisterComponent(
		std::type_index type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	iwu::ref<Archetype>& Space::CreateArchetype(
		std::initializer_list<iwu::ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	Entity Space::CreateEntity(
		const iwu::ref<Archetype>& archetype)
	{
		iwu::ref<EntityData>& entityData = m_entityManager.CreateEntity();
		bool componentsExist = false;
		if (entityData->Archetype && entityData->Archetype->Hash == archetype->Hash) {
			componentsExist = m_componentManager.ReinstateEntityComponents(entityData);
		}

		else {
			entityData->Archetype = archetype;
		}

		if (!componentsExist) {
			entityData->ChunkIndex = m_componentManager.ReserveEntityComponents(entityData);
		}

		return entityData->Entity;
	}

	bool Space::DestroyEntity(
		const Entity& entity)
	{
		iwu::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity);

		return m_componentManager.DestroyEntityComponents(entityData)
			&& m_entityManager.DestroyEntity(entity);
	}

	EntityComponentArray Space::Query(
		const ComponentQuery& query)
	{
		return m_componentManager.Query(m_archetypeManager.MakeQuery(query));
	}
}
