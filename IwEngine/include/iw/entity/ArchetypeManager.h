#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "iw/util/memory/pool_allocator.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace IW {
namespace ECS {
	class ArchetypeManager {
	private:
		std::unordered_map<size_t, iw::ref<Archetype>> m_hashed;
		std::vector<iw::ref<Archetype>> m_archetypes;
		iw::pool_allocator m_pool;
		
	public:
		ArchetypeManager();

		iw::ref<Archetype>& CreateArchetype(
			std::initializer_list<iw::ref<Component>> components);

		iw::ref<ArchetypeQuery> MakeQuery(
			const iw::ref<ComponentQuery>& query);
	};
}

	using namespace ECS;
}
