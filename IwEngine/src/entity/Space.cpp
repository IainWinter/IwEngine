#include "iw/entity/Space.h"

namespace IwEntity {
	Entity Space::CreateEntity() {
		return m_entities.CreateEntity();
	}

	void Space::DestroyEntity(
		Entity entity)
	{
		Archetype old = m_entities.ArchetypeOf(entity);

		m_entities.DestroyEntity(entity);

		for (auto& set : m_components) {
			if (set->HasEntity(entity)) {
				set->DestroyComponent(entity);
			}
		}
	}

	void Space::Sort() {
		for (auto& set : m_components) {
			if (set) {
				set->Sort(m_entities);
			}
		}
	}

	void Space::Clear() {
		m_entities.Clear();
		for (auto& set : m_components) {
			set->Clear();
		}
	}
}
