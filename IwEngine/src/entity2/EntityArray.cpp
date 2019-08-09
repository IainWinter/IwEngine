#include "iw/entity2/EntityArray.h"

namespace IwEntity2 {
	EntityData& EntityArray::CreateEntity() {
		static Entity ne = 1;

		if (m_dead.empty()) {
			EntityData ed{ ne++, Archetype(), 0, true };
			return m_live.emplace_back(ed);
		}

		size_t i = m_dead.front();
		m_dead.pop();

		EntityData& e = m_live.at(i);
		e.Version++;
		e.Arch = Archetype();

		return e;
	}

	EntityData& EntityArray::GetEntityData(
		Entity entity)
	{
		return m_live.at(entity - 1);
	}

	bool EntityArray::DestroyEntity(
		Entity entity)
	{
		if (EntityExists(entity - 1)) {
			EntityData& data = GetEntityData(entity - 1);
			data.Alive = false;
			m_dead.push(data.Id - 1);

			return true;
		}

		return false;
	}

	bool EntityArray::EntityExists(
		Entity entity) const
	{
		return entity - 1 < m_live.size();
	}
}
