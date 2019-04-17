#pragma once

#include "IwEntity.h"
#include <vector>

namespace IwEntity {
	struct EntityData {
		Archetype Archetype;
		std::size_t Count;
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
			m_entities.push_back({ 0, 0 });

			return next++;
		}

		void DestroyEntity(
			Entity entity)
		{
			m_entities.erase(m_entities.begin() + entity);
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
