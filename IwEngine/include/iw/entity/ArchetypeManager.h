#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "iw/util/memory/pool_allocator.h"
#include <initializer_list>
#include <unordered_map>
#include <memory>

namespace iw {
namespace ECS {
	class ArchetypeManager {
	private:
		std::unordered_map<size_t, iw::ref<Archetype>> m_hashed;
		std::vector<iw::ref<Archetype>> m_archetypes;
		iw::pool_allocator m_pool;
		
	public:
		IWENTITY_API
		ArchetypeManager();

		IWENTITY_API
		iw::ref<Archetype>& CreateArchetype(
			std::initializer_list<iw::ref<Component>> components);

		IWENTITY_API
		iw::ref<Archetype>& CreateArchetype(
			const iw::ref<Component>* begin,
			const iw::ref<Component>* end);

		IWENTITY_API
		iw::ref<Archetype>& AddComponent(
			iw::ref<Archetype> archetype, iw::ref<Component> component);

		IWENTITY_API
		iw::ref<Archetype>& RemoveComponent(
			iw::ref<Archetype> archetype, iw::ref<Component> component);

		IWENTITY_API
		iw::ref<ArchetypeQuery> MakeQuery(
			const iw::ref<ComponentQuery>& query);

		IWENTITY_API
		void Clear();
	};
}

	using namespace ECS;
}
