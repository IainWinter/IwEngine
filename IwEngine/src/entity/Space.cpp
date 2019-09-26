#pragma once

#include "iw/entity/Space.h"

namespace IwEntity {
		iwu::ref<const Component> Space::RegisterComponent(
			std::type_index type,
			size_t size)
		{
			return m_componentManager.RegisterComponent(type, size);
		}

		iwu::ref<const Archetype2> Space::CreateArchetype(
			std::initializer_list<iwu::ref<const Component>> components)
		{
			return m_archetypeManager.CreateArchetype(components);
		}

		std::shared_ptr<Entity2> Space::CreateEntity(
			iwu::ref<const Archetype2> archetype)
		{
			auto entity = m_entityManager.CreateEntity();
			entity->Archetype     = archetype;
			entity->Components = m_componentManager.ReserveComponents(entity);

			return entity;
		}

		bool Space::DestroyEntity(
			iwu::ref<const Entity2> entity)
		{
			return m_componentManager.DestroyComponents(entity)
				&& m_entityManager.DestroyEntity(entity->Index);
		}

		EntityComponentArray Space::Query(
			const ComponentQuery& query)
		{
			return m_componentManager.Query(m_archetypeManager.MakeQuery(query));
		}
	}
