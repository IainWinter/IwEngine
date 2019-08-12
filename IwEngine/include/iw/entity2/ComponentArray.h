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
		using Iterator = iwu::pool_allocator::iterator;
		using ConstIterator = iwu::pool_allocator::const_iterator;
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

		Iterator begin() {
			return m_pool.begin();
		}

		Iterator end() {
			return m_pool.end();
		}

		ConstIterator begin() const {
			return m_pool.begin();
		}

		ConstIterator end() const {
			return m_pool.end();
		}

		inline const ArchetypeData& Archetype() const {
			return m_archetypeData;
		}
	};
}
