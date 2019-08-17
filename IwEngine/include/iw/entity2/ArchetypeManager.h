#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "EntityArchetype.h"
#include <vector>

namespace IwEntity2 {
	class ArchetypeManager {
	private:
		std::vector<Archetype> m_archetypes;

	public:
		EntityArchetype AddComponent(
			EntityArchetype& entityArchetype,
			ComponentType componentType,
			size_t componentSize);

		EntityArchetype RemoveComponent(
			EntityArchetype& entityArchetype,
			ComponentType componentType);

		Archetype& GetArchetype(
			EntityArchetype entityArchetype);

		std::vector<EntityArchetype> Query(
			const Archetype& query);

		bool ArchetypeExists(
			EntityArchetype entityArchetype);
	};
}
