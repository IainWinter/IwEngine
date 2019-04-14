#pragma once

#include "IwEntity.h"
#include <vector>

namespace IwEntity {
	struct EntityData {
		Archetype Archetype;
		std::size_t Count;
	};

	struct EntityVector {
		std::vector<EntityData> Entities;

		bool operator()(
			int a,
			int b) const
		{
			int count = Entities[a].Count - Entities[b].Count;
			if (count == 0) {
				int size = Entities[a].Archetype
					- Entities[b].Archetype;

				if (size == 0) {
					return a < b;
				}

				return size >= 0;
			}

			return count >= 0;
		}

		template<
			typename _c>
		Archetype& AssignComponent(
			Entity entity)
		{
			return Entities.at(entity).Archetype |= 1 << Family::type<_c>;
		}

		template<
			typename _c>
		Archetype& UnassignComponent(
			Entity entity)
		{
			return Entities.at(entity).Archetype = ~(Entities.at(entity).Archetype & 1 << Family::type<_c>);
		}
	};
}
