#pragma once

#include <unordered_map>
#include "component_list.h"

namespace iwecs {
	class registry {
	public:
	private:
		struct entity_data {
			std::size_t archetype_id;
			std::size_t component_index;
		};

		std::size_t m_next_entity;
		std::unordered_map<std::size_t, entity_data> m_entities;
		std::unordered_map<std::size_t, icomponent_list> m_components;
	public:
		registry() = default;

		registry(const registry& copy) = delete;
		registry(registry&& copy) = default;

		registry& operator=(const registry& copy) = delete;
		registry& operator=(registry&& copy) = default;

		template<
			typename... _components_t>
		std::size_t create(
			_components_t&&... components)
		{
			using list_t = component_list<_components_t...>;

			std::size_t id = iwutil::archetype<_components_t...>::id;
			list_t& list = ensure_list(id);
			list.push_back(std::forward<_components_t>(components)...);
			
			entity_data data = {
				id,
				list.size() - 1
			};

			m_entities.emplace(++m_next_entity, data);

			return m_next_entity;
		}

		template<
			typename... _components_t>
		std::size_t create(
			const _components_t&... components)
		{
			using list_t = component_list<_components_t...>;

			std::size_t id = iwutil::archetype<_components_t...>::id;
			list_t& list = ensure_list(id);
			list.push_back(components...);

			entity_data data = {
				id,
				list.size() - 1
			};

			m_entities.emplace(++m_next_entity, data);

			return m_next_entity;
		}

		//entity_t create<_components>(_components)
		//	check components for list matching the archetype id
		//		if there is no match: create a new one with the archetype
		//		if there is a  match: get it
		//	forwards the components to the list
		//	creates entity_data with the archetype id and size of the list - 1
		//	adds the entity_data and the next entity id to entities vector
		//	return entity id

		template<
			typename _component_t>
		void assign(
			std::size_t entity,
			_component_t&& component)
		{
			if (m_entities.find(entity) == m_entities.end())
				return;

			entity_data& data = m_entities.at(entity);
			icomponent_list& = m_components[data.archetype_id].
		}

		//void assign<_component>(entity_id, const _component&)
		//	get entity_data from entity_id
		//	get list from components
		//	get id from the list's archetype + _component
		//	check components for list matching the archetype id
		//		if there is no match: create a new one with the new archetype
		//		if there is a  match: get it
		//	get data from old list
		//	add old data + new component to list

		//void assign<_component, _args>(entity_id, const args&)
		//void assign<_component, _args>(entity_id, args&&)  
		//bool destroy(entity_id)
		//bool destroy<_component>(entity_id)
		//view view<_components>()
	private:
		//ensure_list()

		icomponent_list& ensure_list(
			std::size_t archetype_id)
		{
			return m_components[archetype_id];
		}

		template<
			typename... _components_t>
		component_list<_components_t...>& ensure_list() {
			std::size_t id = iwutil::archetype<_components_t>::id;
			return (component_list<_components_t...>&)ensure_list(id);
		}
	};
}


//  p v m
//  1 1 1
//  1 1 1
//
//
//
//