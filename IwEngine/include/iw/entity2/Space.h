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
		std::unordered_map<Entity, void*> m_entities;
		iwu::pool_allocator               m_pool;

	public:
		ComponentArray(size_t pageSize, size_t archetypeSize)
			: m_pool(pageSize, archetypeSize)
		{}

		void* CreateComponents(
			const Entity& entity)
		{
			auto itr = m_entities.find(entity);
			if (itr == m_entities.end()) {
				void* components = m_pool.alloc();
				m_entities.emplace(entity, components);
				return components;
			}

			return nullptr;
		}

		void* GetComponents(
			const Entity& entity)
		{
			auto itr = m_entities.find(entity);
			if (itr != m_entities.end()) {
				return m_entities.at(entity);
			}

			return nullptr;
		}

		size_t ArchetypeSize() const {
			return m_pool.item_size();
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
		_c* CreateComponent(
			const IwEntity::Entity& entity)
		{
			// Find if entity exists
			//  Yes: contiune
			//   No: exit
			auto eitr = m_entities.find(entity);
			if (eitr == m_entities.cend()) {
				return nullptr;
			}

			// Find archetype of entity
			Archetype& archetype  = eitr->second;

			// Find if ComponentArray exists for that archetype
			//  Yes: Get ptr to components
			//   No: Continue with null ptr
			size_t csize = 0;
			void*  cptr  = nullptr;
			auto   citr  = m_components.find(archetype);
			if (citr != m_components.cend()) {
				ComponentArray& cary = m_components.at(archetype);
				csize = cary.ArchetypeSize();
				cptr  = cary.GetComponents(entity);
			}

			// Append component to entity archetype
			ComponentId cid = GetComponentId(typeid(_c));
			archetype ^= cid; //find how to hash multiable ints

			// Find if ComponentArray doesn't exists for new archetype
			//  Yes: Create it
			//   No: Get it
			citr = m_components.find(archetype);
			if (citr == m_components.cend()) {
				citr = m_components.emplace(
					archetype, ComponentArray(1024, csize + sizeof(_c))).first;
			}

			ComponentArray& cary = citr->second;
			void* components = cary.CreateComponents(entity);

			if (cptr) {
				memcpy(components, cptr, csize);
			}

			// If ptr isn't null
			//  Yes: Move data into new Componentarray
			//   No: Do nothing

			// Alloc new components

			return (_c*)((size_t)components + csize);
		}
	};
}
