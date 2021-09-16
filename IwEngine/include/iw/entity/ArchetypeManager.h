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
		std::unordered_map<size_t, Archetype> m_hashed; // there are two copies stored
		std::vector<Archetype> m_archetypes;
		pool_allocator m_pool;
		
	public:
		IWENTITY_API
		ArchetypeManager();

		IWENTITY_API
		Archetype CreateArchetype(
			std::initializer_list<ref<Component>> components);

		IWENTITY_API
		Archetype CreateArchetype(
			const ref<Component>* begin,
			const ref<Component>* end);

		IWENTITY_API
		Archetype AddComponent(
			const Archetype& archetype,
			ref<Component> component);

		IWENTITY_API
		Archetype RemoveComponent(
			const Archetype& archetype,
			ref<Component> component);

		IWENTITY_API
		ArchetypeQuery MakeQuery(
			const ComponentQuery& query);

		IWENTITY_API
		void Clear();

		IWENTITY_API
		const std::vector<Archetype>& Archetypes() const;
	};
}

	using namespace ECS;
}
