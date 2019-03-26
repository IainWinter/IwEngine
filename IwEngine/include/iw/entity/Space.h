#pragma once

#include "iw/util/set/sparse_set.h"
#include "iw/util/type/type_group.h"
#include <vector>
#include <list>

//Idea board 
//
// Space stores a map of subspaces with a category id
// Subspaces store components of their category in diffrent chunks, 1 for each configuration.
//	i.e. if there are 3 possible components there will be 6 chunks -> 001 010 100 011 110 111
// 
// Space
// -> Subspace(Physics)
//	-> Chunk(Velocity)			-> Chunk(Velocity) -> nullptr
//	-> Chunk(Rigidbody)			-> nullptr
//	-> Chunk(Velocity, Rigidbody) -> nullptr
// -> Subspace(Graphics)
//	-> Chunk(Mesh)				-> nullptr
//

namespace IwEntity {
	using Archetype = unsigned long long;
	using Category  = unsigned int;

	class Chunk {
		Archetype archetype;

		void* data;
		std::size_t size;
		std::size_t elementSize;
	};

	class Subspace {
		Archetype archetype;
		Category category;

		std::list<Chunk> chunks;
	};

	class Space {
		iwu::sparse_set<Category, Subspace*> subspaces;
	};
}

namespace IwEntity2 {
	using ComponentId = unsigned int;
	using Entity      = unsigned int;
	using Archetype   = unsigned long long;

	class Space {
	private:
		using EntityMap = iwu::sparse_set<Entity, Archetype>;

		template<
			typename _component_t>
		using ComponentSet = iwu::sparse_set<Entity, _component_t>;

		using ComponentMap = iwu::sparse_set<ComponentId,
			iwu::sparse_set<Entity>*>;

		using ComponentGroup = iwu::type_group<Space>;

		EntityMap    m_entities;
		ComponentMap m_components;

	public:
		~Space() {
			for (auto set : m_components) {
				delete set;
			}
		}

		Entity CreateEntity() {
			static Entity nextEntity = 0;
			m_entities.emplace(++nextEntity, Archetype());

			return nextEntity;
		}

		void DestroyEntity(
			Entity entity)
		{
			if (m_entities.contains(entity)) {
				m_entities.erase(entity);
				for (auto set : m_components) {
					if (set->contains(entity)) {
						set->erase(entity);
					}
				}
			}
		}

		template<
			typename _component_t,
			typename... _args_t>
		void CreateComponent(
			Entity entity,
			const _args_t& ... args)
		{
			AddComponentToArchetype<_component_t>(entity);
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			set.emplace(entity, _component_t{ args... });
		}

		template<
			typename _component_t>
		void DestroyComponent(
			Entity entity)
		{
			RemoveComponentFromArchetype<_component_t>(entity);
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			if (set.contains(entity)) {
				set.erase(entity);
			}
		}

		template<
			typename _component_t>
		_component_t& GetComponent(
			Entity entity)
		{
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			return set.at(entity);
		}
	private:
		template<
			typename _component_t>
		ComponentSet<_component_t>& EnsureSet() {
			unsigned int id = ComponentGroup::type<_component_t>;
			ComponentSet<_component_t>* set = nullptr;
			if (m_components.contains(id)) {
				set = static_cast<ComponentSet<_component_t>*>(
					m_components.at(id));
			}

			else {
				set = new iwu::sparse_set<Entity, _component_t>;
				m_components.emplace(id, set);
			}

			return *set;
		}

		template<
			typename _component_t>
		void AddComponentToArchetype(
			Entity entity)
		{
			unsigned int id = ComponentGroup::type<_component_t>;
			m_entities.at(entity) |= 1UL << id;
		}

		template<
			typename _component_t>
		void RemoveComponentFromArchetype(
			Entity entity)
		{
			unsigned int id = ComponentGroup::type<_component_t>;
			m_entities.at(entity) &= ~(1UL << id);
		}
	};
}
