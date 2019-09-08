#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	class EntityManager {
	private:
		std::vector<Entity2> m_entities;

	public:
		Entity2& CreateEntity(
			const EntityArchetype& archetype)
		{
			return m_entities.emplace_back(m_entities.size(), archetype);
		}
	};
}
