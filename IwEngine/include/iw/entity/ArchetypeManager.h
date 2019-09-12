#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace IwEntity {
	class IWENTITY_API ArchetypeManager {
	private:
		std::unordered_map<size_t, iwu::ref<Archetype2>> m_archetypes;

	public:
		iwu::ref<Archetype2> CreateArchetype(
			std::initializer_list<iwu::ref<const Component>> components);
	};
}
