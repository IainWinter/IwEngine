#include "iw/entity2/ArchetypeArray.h"

namespace IwEntity2 {
	Archetype ArchetypeArray::nextArchetype = 1;

	Archetype ArchetypeArray::AddComponent(
		Archetype& archetype,
		Component componentId,
		size_t componentSize)
	{
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
			auto itr = m_archetypes.begin()/*(archetype) + 1*/;   // See if this needs to start at the beginning. 
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
		// If archetype is empty do nothing
		if (archetype == 0) {
			return archetype;
		}

		// If old archetype
		else {
			ArchetypeData& data = m_archetypes.at(archetype);
			if (data.Count() == 1) {
				return archetype = 0;
			}

			// Find archetype with all its components + the new component
			auto itr = m_archetypes.begin()/*(archetype) + 1*/;   // See if this needs to start at the beginning. 
			for (; itr != m_archetypes.end(); itr++) {    //   If the archetypes are added sequentially, and never removed, then it probly doesn't?
				if (itr->EqualWithout(data, componentId)) {
					return archetype = itr.sparse_index();
				}
			}

			// If not make a new one
			ArchetypeData& newdata = m_archetypes.emplace(nextArchetype, data);
			newdata.RemoveComponent(componentId);
		}

		return archetype = nextArchetype++;
	}

	ArchetypeData& ArchetypeArray::GetArchetypeData(
		Archetype archetype)
	{
		return m_archetypes.at(archetype);
	}

	std::vector<Archetype> ArchetypeArray::FindWith(
		const std::vector<Component>& componentIds)
	{
		std::vector<Archetype> archetypes;
		for (auto itr = m_archetypes.begin(); itr != m_archetypes.end(); itr++)
		{
			for (Component c : componentIds) {
				if (!itr->HasComponent(c)) {
					goto continueloop;
				}
			}

			archetypes.push_back(itr.sparse_index());

		continueloop:
			continue;
		}

		return archetypes;
	}

	bool ArchetypeArray::ArchetypeExists(
		Archetype archetype)
	{
		return m_archetypes.contains(archetype);
	}
}
