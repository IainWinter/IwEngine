#pragma once

#include <assert.h>
#include <unordered_map>
#include "subspace.h"
#include "active_subspace.h"
#include "component/archetype.h"
#include "entity/entity_traits.h"
#include "set/sparse_set.h"
#include "type/type_group.h"
#include "action.h"
#include "component/view.h"

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

		std::vector<iactive_subsapce*> m_subspaces;
		isubspace* m_common;

	public:
		space()
			: m_next_entity(0)
		{}

		~space() {
			for (auto s : m_components) {
				delete s;
			}
		}

		template<
			typename _component_t>
		void make_common() {
			m_common = new subspace<_component_t>(
				get_archetype<_component_t>(),
				make_group<_component_t>());
		}

		template<
			typename... _components_t>
		void make_subspace() {
			archetype a = get_archetype<_components_t...>();

			assert(!m_common->is_similar(a));
			for (auto sub : m_subspaces) {
				assert(!sub->is_similar(a));
			}

			m_subspaces.push_back(
				new active_subspace<_components_t...>(
					a, make_group<_components_t...>()));
		}

		template<
			typename... _components_t>
		void make_action(
			void(*action)(view<_components_t...>&))
		{
			archetype a = get_archetype<_components_t...>();

			assert(m_common->is_similar(a));
			for (auto sub : m_subspaces) {
				if (sub->is_similar(a)) {
					iwutil::iaction* a = new iwutil::action<space, view<_components_t...>&>
						(action, &space::view_components<_components_t...>);
					sub->add_action(a);
				}
			}
		}

		void take_action() {
			for (auto sub : m_subspaces) {
				sub->take_action();
			}
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
			add_component<_component_t>(entity);
		}

		//Erases a component on an entity
		template<
			typename _component_t>
		void destroy(
			entity_type entity)
		{
			assert(valid(entity));
			remove_component<_component_t>(entity);
		}

		//Destroys an entitiy and all its components
		void destroy(
			entity_type entity)
		{
			assert(valid(entity));
			m_expired.push_back(entity);
			m_entities[entity] = archetype();

			size_t type_id = 0;
			for (auto set : m_components) {
				if (has_type(m_entities[entity], type_id)) {
					set->erase(entity);
				}

				type_id++;
			}
		}

		//True if entity is valid, false otherwise
		bool valid(
			entity_type entity) const
		{
			return entity >= 0 && entity < m_entities.size();
		}

		template<
			typename... _components_t>
		view<_components_t...> view_components() {
			return { *ensure<_components_t>()... };
		}
	private:
		//Ensures a set of type _component_t
		template<
			typename _component_t>
		set_t<_component_t>* ensure() {
			size_t id = component_id<_component_t>();
			if (id >= m_components.size()) {
				m_components.resize(id + 1);
			}

			auto itr = m_components.begin() + id;
			if(*itr == nullptr) {
				*itr = new set_t<_component_t>();
			}

			return static_cast<set_t<_component_t>*>(*itr);
		}

		template<
			typename... _components_t>
		group<_components_t...> make_group() {
			return { ensure<_components_t>()... };
		}

		template<
			typename _component_t,
			typename... _args_t>
		void add_component(
			entity_type entity,
			_args_t&&... args)
		{
			add_type<group_type, _component_t>(m_entities[entity]);
			ensure<_component_t>()->emplace(entity, std::forward<_args_t>(args)...);
		}

		template<
			typename _component_t>
		void remove_component(
				entity_type entity)
		{
			remove_type<group_type, _component_t>(m_entities[entity]);
			ensure<_component_t>()->erase(entity);
		}

		template<
			typename _component_t>
		size_t component_id() const {
			return type_id<group_type, _component_t>();
		}

		template<
			typename... _components_t>
		archetype get_archetype() const {
			return make_archetype<group_type, _components_t...>();
		}
	};
}
