#pragma once

#include "iw/entity/Space.h"

namespace IwEntity {
	iwu::ref<Component> Space::RegisterComponent(
		std::type_index type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	iwu::ref<Archetype> Space::CreateArchetype(
		std::initializer_list<iwu::ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	std::shared_ptr<Entity> Space::CreateEntity(
		iwu::ref<Archetype> archetype)
	{
		auto entity = m_entityManager.CreateEntity();
		m_componentManager.ReserveEntityComponents(entity, archetype);

		return entity;
	}

	bool Space::DestroyEntity(
		iwu::ref<Entity> entity)
	{
		return m_componentManager.DestroyEntityComponents(entity)
			&& m_entityManager.DestroyEntity(entity->Index);
	}

	EntityComponentArray Space::Query(
		const ComponentQuery& query)
	{
		return m_componentManager.Query(m_archetypeManager.MakeQuery(query));
	}
}
