#pragma once

#include <unordered_map>
#include "iwecs.h"
#include "component_list.h"
#include "archetype.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<std::size_t, icomponent_list*> m_component_lists;

	public:
		template<
			typename... _components_t>
		std::size_t create(
			_components_t&&... args)
		{
			component_list<_components_t...>& component_list
				= ensure_list<_components_t...>();

			return component_list.push_back(
				std::forward<_components_t>(args)...
			);
		}

		template<
			typename... _components_t>
			std::size_t create(
				const _components_t&... args)
		{
			component_list<_components_t...>& component_list
				= ensure_list<_components_t...>();

			return component_list.push_back(args...);
		}

		bool destroy(
			std::size_t index,
			std::size_t archetype_id)
		{
			if (no_list(archetype_id)) {
				return false;
			}

			return m_component_lists.at(archetype_id)->erase(index);
		}

		template<
			typename... _components_t>
		/* component_view */ void view_components() {

		}
	private:
		template<
			typename... _components_t>
		component_list<_components_t...>& ensure_list() {
			std::size_t id = iwutil::archetype<_components_t...>::id;
			if (no_list(id)) {
				return add_list<_components_t...>();
			}

			return get_list<_components_t...>();
		}

		template<
			typename... _components_t>
		component_list<_components_t...>& add_list() {
			std::size_t id = iwutil::archetype<_components_t...>::id;
			component_list<_components_t...>* clist 
				= new component_list<_components_t...>();

			m_component_lists.emplace(id, clist);

			return *clist;
		}

		template<
			typename... _components_t>
		component_list<_components_t...>& get_list() {
			std::size_t id = iwutil::archetype<_components_t...>::id;
			return *(component_list<_components_t...>*)m_carray[id];
		}

		bool no_list(
			std::size_t id) 
		{
			return m_component_lists.find(id) == m_component_lists.end();
		}
	};
}