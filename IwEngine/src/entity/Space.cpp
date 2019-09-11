#pragma once

#include "iw/entity/Space.h"

namespace IwEntity {
		std::weak_ptr<Component> Space::RegisterComponent(
			std::type_index type,
			size_t size)
		{
			return m_componentManager.RegisterComponent(type, size);
		}

		std::weak_ptr<Archetype2> Space::CreateArchetype(
			std::initializer_list<std::weak_ptr<Component>> components)
		{
			return m_archetypeManager.CreateArchetype(components);
		}

		std::weak_ptr<Entity2> Space::CreateEntity(
			std::weak_ptr<Archetype2> archetype)
		{
			std::weak_ptr<Entity2> entity = m_entityManager.CreateEntity(archetype);
			
			m_componentManager.ReserveComponents(entity);

			return entity;
		}

		bool Space::DestroyEntity(
			std::weak_ptr<Entity2> entity)
		{
			return m_entityManager.DestroyEntity(entity)
				&& m_componentManager.DestroyComponents(entity);
		}
	}
