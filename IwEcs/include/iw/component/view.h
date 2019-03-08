#pragma once

#include <tuple>
#include "iw/entity/entity_traits.h"
#include "iw/set/sparse_set.h"

namespace iwent {
	template<
		typename... _components_t>
	class view {
	private:
		template<
			typename _component_t>
			using set_t = iwutil::sparse_set<entity_type, _component_t>;

		std::tuple<set_t<_components_t>&...> m_components;

	public:
		view(set_t<_components_t>&... components)
			: m_components(components...)
		{}
	};
}
