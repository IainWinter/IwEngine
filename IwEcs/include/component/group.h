#pragma once

#include <tuple>
#include "archetype.h"
#include "entity/entity_traits.h"
#include "set/sparse_set.h"
#include "type/type_index.h"

namespace iwent {
	template<
		typename... _components_t>
	class group {
	private:
		template<
			typename _component_t>
		using set_t = iwutil::sparse_set<entity_type, _component_t>;

		std::tuple<set_t<_components_t>*...> m_components;

	public:
		group(set_t<_components_t>*... components)
			: m_components(components...)
		{}
	};
}
