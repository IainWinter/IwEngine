#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace IwEntity {
	class IWENTITY_API ArchetypeManager {
	private:
		std::unordered_map<size_t, iwu::ref<Archetype>> m_hashed;
		std::vector<iwu::ref<Archetype>> m_archetypes;

	public:
		iwu::ref<Archetype>& CreateArchetype(
			std::initializer_list<iwu::ref<Component>> components);

		iwu::ref<ArchetypeQuery> MakeQuery(
			const iwu::ref<ComponentQuery>& query);
	};
}
