#pragma once

#include "IwEntity.h"
#include "ArchetypeData.h"
#include "iw/util/set/sparse_set.h"
#include <vector>

namespace IwEntity2 {
	class ArchetypeArray {
	private:
		iwu::sparse_set<Archetype, ArchetypeData> m_archetypes;

		static Archetype nextArchetype;

	public:
		Archetype AddComponent(
			Archetype& archetype,
			Component componentId,
			size_t componentSize);

		Archetype RemoveComponent(
			Archetype& archetype,
			Component componentId);

		ArchetypeData& GetArchetypeData(
			Archetype archetype);

		std::vector<Archetype> FindWith(
			const std::vector<Component>& componentIds);

		bool ArchetypeExists(
			Archetype archetype);
	};
}
