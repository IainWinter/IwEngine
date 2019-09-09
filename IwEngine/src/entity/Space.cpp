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

		size_t Space::CreateEntity(
			std::weak_ptr<Archetype2> archetype)
		{
			Entity2& entity = m_entityManager.CreateEntity(archetype);
			entity.ComponentData = m_componentManager.CreateComponents(entity);

			return entity.Index;
		}

		bool Space::DestroyEntity(
			size_t entityIndex)
		{
			Entity2& entity = m_entityManager.GetEntity(entityIndex);
			return  m_entityManager.DestroyEntity(entityIndex)
				&& m_componentManager.DestroyComponents(entity);
		}
	}
