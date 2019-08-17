#include "iw/entity2/EntityManager.h"

namespace IwEntity2 {
	EntityIndex EntityManager::CreateEntity() {
		static EntityIndex nextEntity = 1;

		if (m_dead.empty()) {
			m_entities.emplace_back(nextEntity);
			return nextEntity++;
		}

		EntityIndex e = m_dead.front();
		m_dead.pop();

		Entity& data = m_entities.at(e);
		data.Version++;
		data.Archetype = 0;

		return data.Index;
	}

	bool EntityManager::DestroyEntity(
		EntityIndex entity)
	{
		if (EntityExists(entity)) {
			Entity& data = GetEntityData(entity);
			data.Alive = false;

			m_dead.push(entity - 1);
			return true;
		}

		return false;
	}

	Entity& EntityManager::GetEntityData(
		EntityIndex entity)
	{
		return m_entities.at(entity - 1);
	}

	bool EntityManager::EntityExists(
		EntityIndex entity) const
	{
		return entity - 1 < m_entities.size();
	}
}
