#include "iw/entity/EntityManager.h"

namespace IwEntity {
	Entity2& EntityManager::CreateEntity(
		std::weak_ptr<Archetype2> archetype)
	{
		if (!m_dead.empty()) {
			Entity2& dead = m_entities.at(m_dead.front());
			m_dead.pop();

			dead.Alive = true;
			dead.Version++;
			
			dead.Archetype = archetype;
			dead.ComponentData.reset();

			return dead;
		}

		return m_entities.emplace_back(m_entities.size(), archetype);
	}

	bool EntityManager::DestroyEntity(
		size_t index)
	{
		if (index < m_entities.size()) {
			Entity2& entity = m_entities.at(index);
			if (entity.Alive) {
				entity.Alive = false;
				m_dead.push(index);

				return true;
			}
		}

		return false;
	}

	Entity2& EntityManager::GetEntity(
		size_t entityIndex)
	{
		return m_entities.at(entityIndex);
	}
}
