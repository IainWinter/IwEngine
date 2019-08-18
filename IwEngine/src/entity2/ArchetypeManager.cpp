#include "iw/entity2/ArchetypeManager.h"

namespace IwEntity2 {
	EntityArchetype ArchetypeManager::nextArchetype = 1;

	EntityArchetype ArchetypeManager::AddComponent(
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

	EntityArchetype ArchetypeManager::RemoveComponent(
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

	Archetype& ArchetypeManager::GetArchetype(
		EntityArchetype entityArchetype)
	{
		return m_archetypes.at(entityArchetype - 1);
	}

	std::vector<EntityArchetype> ArchetypeManager::FindWith(
		std::initializer_list<ComponentType> componentTypes)
	{
		std::vector<EntityArchetype> archetypes;
		for (size_t i = 0; i < m_archetypes.size(); i++) {
			Archetype& archetype = m_archetypes[i];
			for (ComponentType c : componentTypes) {
				if (!archetype.HasComponent(c)) {
					goto continueloop;
				}
			}

			archetypes.push_back(i);

continueloop:
			continue;
		}

		return archetypes;
	}

	bool ArchetypeManager::ArchetypeExists(
		EntityArchetype entityArchetype)
	{
		return entityArchetype - 1 < m_archetypes.size();
	}
}