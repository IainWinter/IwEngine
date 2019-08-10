#pragma once

#include "IwEntity.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/util/set/sparse_set.h"
//#include <unordered_map>

namespace IwEntity2 {
	class ComponentArray {
	private:
		iwu::sparse_set<Entity, void*> m_entities;
		//std::unordered_map<Entity, void*> m_entities;
		iwu::pool_allocator               m_pool;

	public:
		ComponentArray(
			size_t pageSize,
			size_t archetypeSize);

		void* CreateComponents(
			Entity entity);

		bool DestroyComponents(
			Entity entity);

		void* GetComponents(
			Entity entity);

		size_t ArchetypeSize() const;
	};
}
