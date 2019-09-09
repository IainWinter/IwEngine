#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace IwEntity {
	class IWENTITY_API ArchetypeManager {
	private:
		std::unordered_map<size_t, std::shared_ptr<Archetype2>> m_archetypes;

	public:
		std::weak_ptr<Archetype2> CreateArchetype(
			std::initializer_list<std::weak_ptr<Component>> components);
	};
}
