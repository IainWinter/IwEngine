#pragma once

#include "IwEntity.h"
#include <vector>

namespace IwEntity {
	struct EntityData {
		Archetype Archetype;
		unsigned int Count;
		unsigned int Version;
		bool Dead;
	};

	class EntityArray {
	private:
		std::vector<EntityData> m_entities;

	public:
		bool operator()(
			int a,
			int b) const
		{
			int count = m_entities[a].Count - m_entities[b].Count;
			if (count == 0) {
				int size = m_entities[a].Archetype
					- m_entities[b].Archetype;

				if (size == 0) {
					return a < b;
				}

				return size >= 0;
			}

			return count >= 0;
		}

		Entity CreateEntity() {
			static Entity next = Entity();

			Entity entity = next;
			for (size_t e = 0; e < m_entities.size(); e++) {
				if (m_entities.at(e).Dead) {
					entity = e;
					break;
				}
			}

			if (entity == next) {
				m_entities.push_back({ 0, 0 });
				return next++;
			}

			EntityData& ed = m_entities.at(entity);
			ed.Dead      = false;
			ed.Archetype = Archetype();
			ed.Version++;

			return entity;
		}

		void DestroyEntity(
			Entity entity)
		{
			m_entities.at(entity).Dead = true;
		}

		void Clear() {
			m_entities.clear();
		}

		Archetype& ArchetypeOf(
			Entity entity)
		{
			return m_entities.at(entity).Archetype;
		}

		template<
			typename _c>
		Archetype& AssignComponent(
			Entity entity)
		{
			return ArchetypeOf(entity) |= 1 << ComponentFamily::type<_c>;
		}

		template<
			typename _c>
		Archetype& UnassignComponent(
			Entity entity)
		{
			return ArchetypeOf(entity) &= ~(ArchetypeOf(entity) & 1 << ComponentFamily::type<_c>);
		}
	};
}
