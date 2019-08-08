#pragma once

#include "iw/entity/IwEntity.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>
#include <cstddef>
#include <malloc.h>
#include <typeindex>
#include <vector>

namespace IwEntity2 {
	using ComponentId = int;  
	using Archetype   = int;
	using Entity      = IwEntity::Entity;

	struct ComponentArray {
		iwu::pool_allocator Pool;

		ComponentArray(std::size_t pageSize, std::size_t archetypeSize)
			: Pool(pageSize, archetypeSize)
		{}
	};

	class Space {
	private:
		std::unordered_map<Entity, Archetype>            m_entities;
		std::unordered_map<Archetype, ComponentArray>    m_components;
		std::unordered_map<std::type_index, ComponentId> m_componentIds;

		ComponentId GetComponentId(
			std::type_index&& type)
		{
			static int nextId = 0;
			auto itr = m_componentIds.find(type);
			if (itr == m_componentIds.end()) {
				itr = m_componentIds.emplace(type, nextId++).first;
			}

			return itr->second;
		}

	public:
		IwEntity::Entity CreateEntity() {
			static IwEntity::Entity e = IwEntity::Entity(1);

			m_entities.emplace(e, Archetype());
			return e++;
		}

		// If there is no page with this component create it
		// If this entity exists in other pages
		//   Find or and make new page with that archetype and move entity into it

		template<
			typename _c>
		void CreateComponent(
			IwEntity::Entity entity)
		{
			auto eitr = m_entities.find(entity);
			if (eitr == m_entities.cend()) {
				return;
			}

			Archetype& arch = eitr->second;
			ComponentId cid = GetComponentId(typeid(_c));

			arch ^= cid; //find how to hash multiable ints

			auto citr = m_components.find(arch);
			if (citr == m_components.cend()) {
				citr = m_components.emplace(
					arch, ComponentArray(1024, sizeof(_c))).first;
			}

			ComponentArray& chunk = citr->second;
			_c* component = chunk.Pool.alloc<_c>();

			LOG_INFO << "Got";
		}
	};
}
