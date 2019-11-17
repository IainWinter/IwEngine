#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace IW {
inline namespace ECS {
	class IWENTITY_API ArchetypeManager {
	private:
		std::unordered_map<size_t, iw::ref<Archetype>> m_hashed;
		std::vector<iw::ref<Archetype>> m_archetypes;

	public:
		iw::ref<Archetype>& CreateArchetype(
			std::initializer_list<iw::ref<Component>> components);

		iw::ref<ArchetypeQuery> MakeQuery(
			const iw::ref<ComponentQuery>& query);
	};
}
}
