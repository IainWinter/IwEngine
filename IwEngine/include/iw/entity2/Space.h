#pragma once

#include "iw/entity/IwEntity.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>
#include <cstddef>
#include <malloc.h>
#include <typeindex>
#include <vector>
#include <algorithm>

namespace IwEntity2 {
	using ComponentId = int;  
	using Archetype   = int;
	using Entity      = IwEntity::Entity;

	class ComponentArray {
	private:
		// meta
		std::vector<Entity> m_entities;
		iwu::pool_allocator m_pool;

	public:
		ComponentArray(std::size_t pageSize, std::size_t archetypeSize)
			: m_pool(pageSize, archetypeSize)
		{}

		void* CreateComponents(
			const Entity& entity)
		{
			auto itr = std::find(m_entities.begin(), m_entities.end(), entity);
			if (itr == m_entities.end()) {
				m_entities.insert(
					std::upper_bound(m_entities.begin(), m_entities.end(), entity),
					entity
				);

				return m_pool.alloc();
			}

			return nullptr;
		}
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
			const IwEntity::Entity& entity)
		{
			// Find if entity exists
			//  Yes: contiune
			//   No: exit
			auto eitr = m_entities.find(entity);
			if (eitr == m_entities.cend()) {
				return;
			}

			// Find archetype of entity
			Archetype& archetype  = eitr->second;

			// Find if ComponentArray exists for that archetype
			//  Yes: Get ptr to components
			//   No: Continue with null ptr
			void* cptr = nullptr;
			auto  citr = m_components.find(archetype);
			if (citr != m_components.cend()) {
				ComponentArray& cary = m_components.at(archetype);
				cptr = cary.GetComponents(entity);
			}

			// Append component to entity archetype
			archetype ^= cid; //find how to hash multiable ints

			// Find if ComponentArray doesn't exists for new archetype
			//  Yes: Create it
			//   No: Get it
			citr = m_components.find(archetype);
			if (citr == m_components.cend()) {
				citr = m_components.emplace(
					archetype, ComponentArray(1024, sizeof(_c))).first;
			}

			ComponentArray& cary = city->second;
			void* components = cary.CreateComponents(entity);

			if (cptr) {
				
			}


			// Alloc new components
			// If ptr isn't null
			//  Yes: Move data into new Componentarray
			//   No: Do nothing






			ComponentId cid = GetComponentId(typeid(_c));



			ComponentArray& chunk = citr->second;
			_c* components = (_c*)chunk.CreateComponents();

			LOG_INFO << "Got";
		}
	};
}
