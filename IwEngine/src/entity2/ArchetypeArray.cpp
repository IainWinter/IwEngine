#include "iw/entity2/ArchetypeArray.h"

namespace IwEntity2 {
	EntityArchetype ArchetypeArray::nextArchetype = 1;

	EntityArchetype ArchetypeArray::AddComponent(
		EntityArchetype& entityArchetype,
		ComponentType componentType,
		size_t componentSize)
	{
		// If adding a component to a uninitialized archetype
		if (entityArchetype == 0) {
			// If an archetype exists with only the new component, return it
			for (EntityArchetype ea = 1; ArchetypeExists(ea); ea++) {
				Archetype& archetype = GetArchetype(ea);
				if (archetype.Count() == 1 && archetype.HasComponent(componentType)) {
					return entityArchetype = ea;
				}
			}

			// If not make a new one
			Archetype& archetype = m_archetypes.emplace_back();
			archetype.AddComponent(componentType, componentSize);
		}

		// If adding a component to an existing archetype
		else {
			Archetype& oldArchetype = GetArchetype(entityArchetype);

			// If an archetype exists with the same components + the new component, return it
			for (EntityArchetype ea = 1; ArchetypeExists(ea); ea++) {
				Archetype& archetype = GetArchetype(ea);
				if (archetype.EqualWith(oldArchetype, componentType)) {
					return entityArchetype = ea;
				}
			}

			// If not make a new one
			Archetype& archetype = m_archetypes.emplace_back(oldArchetype);
			archetype.AddComponent(componentType, componentSize);
		}

		// Return the new archetype
		return entityArchetype = nextArchetype++;
	}

	EntityArchetype ArchetypeArray::RemoveComponent(
		EntityArchetype& entityArchetype,
		ComponentType componentType)
	{
		// If removing a component from a uninitialized archetype, do nothing and exit
		if (entityArchetype == 0) {
			return entityArchetype;
		}

		// if removing the last component, return null archetype
		Archetype& oldData = GetArchetype(entityArchetype);
		if (oldData.Count() == 1) {
			return entityArchetype = 0;
		}

		// If an archetype exists with the same components - the component, return it
		for (EntityArchetype ea = 1; ArchetypeExists(ea); ea++) {
			Archetype& archetype = GetArchetype(ea);
			if (archetype.EqualWithout(oldData, componentType)) {
				return entityArchetype = ea;
			}
		}

		// If not make a new one
		Archetype& archetype = m_archetypes.emplace_back(oldData);
		archetype.RemoveComponent(componentType);

		// Return the new archetype
		return entityArchetype = nextArchetype++;
	}

	Archetype& ArchetypeArray::GetArchetype(
		EntityArchetype entityArchetype)
	{
		return m_archetypes.at(entityArchetype - 1);
	}

	std::vector<EntityArchetype> ArchetypeArray::Query(
		const Archetype& query)
	{
		std::vector<EntityArchetype> archetypes;
		for (EntityArchetype ea = 1; ArchetypeExists(ea); ea++) {  // find other way to do this
			Archetype& archetype = m_archetypes[ea];
			for (ComponentTypeInfo c : query.Layout()) {
				if (!archetype.HasComponent(c.Type)) {
					goto continueloop;
				}
			}

			archetypes.push_back(ea);

continueloop:
			continue;
		}

		return archetypes;
	}

	bool ArchetypeArray::ArchetypeExists(
		EntityArchetype entityArchetype)
	{
		return entityArchetype - 1 < m_archetypes.size();
	}
}
