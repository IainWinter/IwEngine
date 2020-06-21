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
		std::unordered_map<size_t, ref<Archetype>> m_hashed;
		std::vector<ref<Archetype>> m_archetypes;
		pool_allocator m_pool;
		
	public:
		IWENTITY_API
		ArchetypeManager();

		IWENTITY_API
		ref<Archetype>& CreateArchetype(
			std::initializer_list<ref<Component>> components);

		IWENTITY_API
		ref<Archetype>& CreateArchetype(
			const ref<Component>* begin,
			const ref<Component>* end);

		IWENTITY_API
		ref<Archetype>& AddComponent(
			ref<Archetype> archetype, ref<Component> component);

		IWENTITY_API
		ref<Archetype>& RemoveComponent(
			ref<Archetype> archetype, ref<Component> component);

		IWENTITY_API
		ref<ArchetypeQuery> MakeQuery(
			const ref<ComponentQuery>& query);

		IWENTITY_API
		void Clear();

		IWENTITY_API
		const std::vector<ref<Archetype>>& Archetypes() const;
	};
}

	using namespace ECS;
}
