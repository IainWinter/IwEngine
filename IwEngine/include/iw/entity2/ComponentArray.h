#pragma once

#include "IwEntity.h"
#include "ArchetypeData.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/util/set/sparse_set.h"
//#include <unordered_map>
		//std::unordered_map<Entity, void*> m_entities;

namespace IwEntity2 {
	class ComponentArray {
	public:
		using iterator = iwu::pool_allocator::iterator;
	private:
		iwu::sparse_set<Entity, void*> m_entities;
		ArchetypeData                  m_archetypeData;
		iwu::pool_allocator            m_pool;

	public:
		ComponentArray(
			size_t pageSize,
			const ArchetypeData& archetypeData);

		void* CreateComponents(
			Entity entity);

		bool DestroyComponents(
			Entity entity);

		void* GetComponents(
			Entity entity);

		void* CopyInto(
			ComponentArray& source,
			Entity entity);

		iterator begin() const {
			m_pool.begin();
		}

		iterator end() const {
			m_pool.end();
		}

		inline const ArchetypeData& Archetype() const {
			return m_archetypeData;
		}
	};
}
