#include "iw/entity2/ArchetypeArray.h"

namespace IwEntity2 {
	Archetype ArchetypeArray::AddComponent(
		Archetype& archetype,
		Component componentId,
		size_t componentSize)
	{
		static Archetype nextArchetype = 1;

		// If a new archetype
		if (archetype == 0) {
			// If one already exists with only the new component return it
			auto itr = m_archetypes.begin();
			for (; itr != m_archetypes.end(); itr++) {
				if (itr->Count() == 1 && itr->HasComponent(componentId)) {
					return archetype = itr.sparse_index();
				}
			}

			// If not make a new one
			ArchetypeData& data = m_archetypes.emplace(nextArchetype);
			data.AddComponent(componentId, componentSize);
		}

		// If old archetype
		else {
			ArchetypeData& data = m_archetypes.at(archetype);

			// Find archetype with all its components + the new component
			auto itr = m_archetypes.find(archetype) + 1;   // See if this needs to start at the beginning. 
			for (; itr != m_archetypes.end(); itr++) {    //   If the archetypes are added sequentially, and never removed, then it probly doesn't?
				if (itr->EqualWith(data, componentId)) {
					return archetype = itr.sparse_index();
				}
			}

			// If not make a new one
			ArchetypeData& newdata = m_archetypes.emplace(nextArchetype, data);
			newdata.AddComponent(componentId, componentSize);
		}

		// Return the new archetyp
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
