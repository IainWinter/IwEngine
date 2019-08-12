#include "iw/entity2/EntityArray.h"

namespace IwEntity2 {
	Entity EntityArray::CreateEntity() {
		static Entity nextEntity = 1;

		if (m_dead.empty()) {
			m_entities.emplace_back(nextEntity);
			return nextEntity++;
		}

		Entity e = m_dead.front();
		m_dead.pop();

		EntityData& data = m_entities.at(e);
		data.Version++;
		data.Archetype = 0;

		return data.Id;
	}

	bool EntityArray::DestroyEntity(
		Entity entity)
	{
		if (EntityExists(entity)) {
			EntityData& data = GetEntityData(entity);
			data.Alive = false;

			m_dead.push(entity - 1);
			return true;
		}

		return false;
	}

	EntityData& EntityArray::GetEntityData(
		Entity entity)
	{
		return m_entities.at(entity - 1);
	}

	bool EntityArray::EntityExists(
		Entity entity) const
	{
		return entity - 1 < m_entities.size();
	}
}
