#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include <vector>

namespace IwEntity2 {
	class ArchetypeArray {
	private:
		std::vector<Archetype> m_archetypes;

		static EntityArchetype nextArchetype;

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

		std::vector<EntityArchetype> FindWith(
			std::initializer_list<ComponentType> componentTypes);

		bool ArchetypeExists(
			EntityArchetype entityArchetype);
	};
}
