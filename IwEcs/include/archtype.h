#pragma once

#include <vector>
#include "chunk.h"

namespace iwecs {
	template<typename... _components_t>
	class archtype {
	private:
		using tup_t = std::tuple<_components_t...>;

		tup_t m_components;
	public:
		archtype(_components_t&&... args)
			: m_components(std::forward<_components_t&&>(args)...) {}

		tup_t& components() {
			return m_components;
		}
	};
}