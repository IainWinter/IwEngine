#pragma once

#include <assert.h>
#include <unordered_map>
#include "subspace.h"
#include "component/archetype.h"
#include "entity/entity_traits.h"
#include "set/sparse_set.h"
#include "type/type_group.h"

namespace iwent {
	template<
		typename _group>
	class space {
	public:
		using group_type = _group;
	private:
		template<
			typename _component_t>
		using set_t = iwutil::sparse_set<entity_type, _component_t>;

		std::vector<iwutil::sparse_set<entity_type>*> m_components;

		std::vector<archetype> m_entities;
		std::vector<entity_type> m_expired;
		entity_type m_next_entity;

		std::vector<isubsapce*> m_subspaces;
		isubsapce* m_common;

	public:
		space()
			: m_next_entity(0)
		{}

		template<
			typename _component_t>
		void make_common() {
			group<_component_t> group(ensure<_component_t>());
			m_common = new subspace<_component_t>(get_id<_component_t>(), group);
		}

		template<
			typename... _components_t>
		void make_sspace() {
			archetype a = get_id<_components_t...>();

			assert(!m_common->is_similar(a));
			for (auto sub : m_subspaces) {
				assert(!sub->is_similar(a));
			}

			group<_components_t...> group(ensure<_components_t>()...);
			m_subspaces.push_back(new subspace<_components_t...>(a, group));
		}

		//Creates an empty entity
		entity_type create() {
			entity_type entity;
			if (m_expired.size() != 0) {
				entity = m_expired.back();
				m_expired.pop_back();
				m_entities[entity] = archetype();
			}

			else {
				m_entities.push_back(archetype());
				entity = m_next_entity++;
			}

			return entity;
		}

		//Assigns components to entity 
		template<
			typename _component_t,
			typename... _args_t>
		void assign(
			entity_type entity,
			_args_t&&... args)
		{
			assert(valid(entity));
			ensure<_component_t>()->emplace(entity, std::forward<_args_t>(args)...);
		}

		//Erases a component on an entity
		template<
			typename _component_t>
		void destroy(
			entity_type entity)
		{
			assert(valid(entity));
			ensure<_component_t>()->erase(entity);
		}

		//Destroys an entitiy and all its components
		void destroy(
			entity_type entity)
		{
			assert(valid(entity));
			m_expired.push_back(entity);

			for (auto set : m_components) {
				set->erase(entity);
			}
		}

		//True if entity is valid, false otherwise
		bool valid(
			entity_type entity) const
		{
			return entity >= 0 && entity < m_entities.size();
		}

		//Makes an archetype from the types
		template<
			typename... _t>
		archetype get_id() const {
			return iwent::make_archetype<group_type, _t...>();
		}
	private:
		//Ensures a set of type _component_t
		template<
			typename _component_t>
		set_t<_component_t>* ensure() {
			size_t id = type_id<group_type, _component_t>();
			if (id >= m_components.size()) {
				m_components.resize(id + 1);
			}

			auto itr = m_components.begin() + id;
			if(*itr == nullptr) {
				*itr = new set_t<_component_t>();
			}

			return static_cast<set_t<_component_t>*>(*itr);
		}
	};
}
