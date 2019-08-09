#pragma once

#include "iw/entity2/IwEntity.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>

namespace IwEntity2 {
	class ComponentArray {
	private:
		std::unordered_map<Entity, void*> m_entities;
		iwu::pool_allocator               m_pool;

	public:
		ComponentArray(
			size_t pageSize,
			size_t archetypeSize);

		void* CreateComponents(
			Entity entity);

		void DestroyComponents(
			Entity entity);

		void* GetComponents(
			Entity entity);

		size_t ArchetypeSize() const;
	};
}
