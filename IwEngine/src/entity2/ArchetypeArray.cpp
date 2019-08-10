#include "iw/entity2/ArchetypeArray.h"

namespace IwEntity2 {
	Archetype ArchetypeArray::AddComponent(
		Archetype& archetype,
		Component componentId,
		size_t componentSize)
	{
		static Archetype nextArchetype = 1;

		// If new archetype
		// Search for archetype with only the component in it
		// or make a new archetype
		if (archetype == 0) {
			auto itr = m_archetypes.begin();
			for (; itr != m_archetypes.end(); itr++) {
				if (itr->Count() == 1 && itr->HasComponent(componentId)) {
					return archetype = itr.sparse_index();
				}
			}

			ArchetypeData& data = m_archetypes.emplace(nextArchetype);
			data.AddComponent(componentId, componentSize);

			return archetype = nextArchetype++;
		}

		ArchetypeData& data = m_archetypes.at(archetype);

		auto itr = m_archetypes.begin();
		for (; itr != m_archetypes.end(); itr++) {
			if (itr->Count() == 1 + data.Count()) {
				//if (*itr == newdata) {
					return archetype = itr.sparse_index();
				//}
			}
		}

		ArchetypeData& newdata = m_archetypes.emplace(nextArchetype, data);
		newdata.AddComponent(componentId, componentSize);

		return archetype = nextArchetype++;
	}

	Archetype ArchetypeArray::RemoveComponent(
		Archetype& archetype,
		Component componentId)
	{
		return Archetype();
	}

	ArchetypeData& ArchetypeArray::GetArchetypeData(
		Archetype archetype)
	{
		return m_archetypes.at(archetype);
	}

	bool ArchetypeArray::ArchetypeExists(
		Archetype archetype)
	{
		return m_archetypes.contains(archetype);
	}
}
