#pragma once

#include <vector>
#include "component/bag_view.h"
#include "type/type_group.h"
#include "set/sparse_set.h"

namespace iwecs {
	template<
		typename _index_t>
	class bag {
	private:
		template<typename _t>
		using set_t = iwutil::sparse_set<_index_t, _t>;

		struct component_group_tag;
		using component_group_t = iwutil::type_group<component_group_tag>;

		std::vector<iwutil::sparse_set<_index_t>*> m_sets;

	public:
		template<
			typename _component_t,
			typename... _args_t>
		void emplace(
			_index_t key,
			_args_t&&... args)
		{
			ensure_set<_component_t>().emplace(key, std::forward<_args_t>(args)...);
		}

		template<
			typename _component_t>
		void erase(
			_index_t key)
		{
			ensure_set<_component_t>().erase(key);
		}

		template<
			typename... _components_t>
		bag_view<_index_t, _components_t...> view() {
			return { &ensure_set<_components_t>()... };
		}
	private:
		template<
			typename _component_t>
		set_t<_component_t>& ensure_set() {
			int index = component_group_t::type<_component_t>;
			if (index == m_sets.size()) {
				m_sets.push_back(new set_t<_component_t>());
			}

			return static_cast<set_t<_component_t>&>(*m_sets[index]);
		}
	};
}