#pragma once

#include "set/sparse_set.h"
#include <tuple>

namespace iwecs {
	template<
		typename _index_t,
		typename... _components_t>
	class bag_view {
	public:
	private:
		template<typename _t>
		using set_t = iwutil::sparse_set<_index_t, _t>;

		const std::tuple<set_t<_components_t>*...> m_sets;

	public:
		bag_view(
			set_t<_components_t>*... sets)
			: m_sets(sets...)
		{}

	private:
	};
}