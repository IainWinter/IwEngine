#pragma once

#include <unordered_map>
#include "component_list.h"

namespace iwecs {
	class registry {
	public:
	private:
		struct entity_data {
			size_t archetype_id;
			size_t component_index;
		};

		std::unordered_map<size_t, icomponent_list> m_components;
		std::unordered_map<size_t, entity_data> m_entities;
	public:
		registry() = default;

		registry(const registry& copy) = delete;
		registry(registry&& copy) = default;

		registry& operator=(const registry& copy) = delete;
		registry& operator=(registry&& copy) = default;

		//entity_t create<_components>(_components)
		//	check components for list matching the archetype id
		//		if there is no match: create a new one with the archetype
		//		if there is a  match: get it
		//	forwards the components to the list
		//	creates entity_data with the archetype id and size of the list - 1
		//	adds the entity_data and the next entity id to entities vector
		//	return entity id

		//void assign<_component>(entity_id, const _component&)
		//	get entity_data from entity_id
		//	get list from components
		//	get id from the list's archetype + _component
		//	check components for list matching the archetype id
		//		if there is no match: create a new one with the new archetype
		//		if there is a  match: get it
		//	get data from old list
		//	add 

		//void assign<_component, _args>(entity_id, const args&)
		//void assign<_component, _args>(entity_id, args&&)  
		//bool destroy(entity_id)
		//bool destroy<_component>(entity_id)
		//view view<_components>()
	private:

	};
}