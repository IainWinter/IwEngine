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
			while (itr != m_archetypes.end()) {
				if (itr->Count() == 1 && itr->HasComponent(componentId)) {
					return archetype = itr.sparse_index();
				}

				++itr;
			}

			// If not make a new one
			ArchetypeData& data = m_archetypes.emplace(nextArchetype);
			data.AddComponent(componentId, componentSize);
		}

		// If old archetype
		else {
			ArchetypeData& oldData = m_archetypes.at(archetype);

			// Find archetype with all its components + the new component
			auto itr = m_archetypes.begin()/*(archetype) + 1*/;   // See if this needs to start at the beginning. 
			while (itr != m_archetypes.end()) {                    //   If the archetypes are added sequentially, and never removed, then it probly doesn't?
				if (itr->EqualWith(oldData, componentId)) {
					return archetype = itr.sparse_index();
				}

				++itr;
			}

			// If not make a new one
			ArchetypeData& data = m_archetypes.emplace(nextArchetype, oldData);
			data.AddComponent(componentId, componentSize);
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
			ArchetypeData& oldData = m_archetypes.at(archetype);
			if (oldData.Count() == 1) {
				return archetype = 0;
			}

			// Find archetype with all its components + the new component
			auto itr = m_archetypes.begin()/*(archetype) + 1*/;   // See if this needs to start at the beginning. 
			while(itr != m_archetypes.end()) {    //   If the archetypes are added sequentially, and never removed, then it probly doesn't?
				if (itr->EqualWithout(oldData, componentId)) {
					return archetype = itr.sparse_index();
				}

				++itr;
			}

			// If not make a new one
			ArchetypeData& data = m_archetypes.emplace(nextArchetype, oldData);
			data.RemoveComponent(componentId);
		}

		return archetype = nextArchetype++;
	}

	ArchetypeData& ArchetypeArray::GetArchetypeData(
		Archetype archetype)
	{
		return m_archetypes.at(archetype);
	}

	std::vector<Archetype> ArchetypeArray::FindWith(
		std::initializer_list<Component> componentIds)
	{
		std::vector<Archetype> archetypes;
		auto itr = m_archetypes.begin();
		while(itr != m_archetypes.end()) {
			for (Component c : componentIds) {
				if (!itr->HasComponent(c)) {
					goto continueloop;
				}
			}

			archetypes.push_back(itr.sparse_index());

continueloop:
			++itr;
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
