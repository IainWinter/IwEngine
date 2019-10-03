#pragma once

#include "iw/entity/Space.h"
#include "iw/log/logger.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<Component>& Space::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	iwu::ref<Component>& Space::GetComponent(
		ComponentType type)
	{
		return m_componentManager.GetComponent(type);
	}

	iwu::ref<ComponentQuery> Space::MakeQuery(
		std::initializer_list<iwu::ref<Component>> components)
	{
		size_t bufSize = sizeof(ComponentQuery)
			+ sizeof(iwu::ref<Component>)
			* components.size();

		ComponentQuery* buf = (ComponentQuery*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		buf->Count = components.size();

		size_t i = 0;
		for (iwu::ref<Component> component : components) {
			buf->Components[i++] = component;
		}

		return iwu::ref<ComponentQuery>(buf, free);
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
		size_t index)
	{
		iwu::ref<EntityData>& entityData = m_entityManager.GetEntityData(index);
		if (!entityData->Entity.Alive) {
			LOG_WARNING << "Tried to delete dead entity!";
			return false;
		}

		return m_entityManager.DestroyEntity(index)
			&& m_componentManager.DestroyEntityComponents(entityData);
	}

	EntityComponentArray Space::Query(
		const iwu::ref<ComponentQuery>& query)
	{
		return m_componentManager.Query(query, m_archetypeManager.MakeQuery(query));
	}
}
