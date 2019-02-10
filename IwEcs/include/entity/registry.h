#pragma once

#include <vector>
#include "component/component_bag.h"

namespace iwecs {
	template<
		typename _entity_t>
	class registry {
	public:
		using entity_type = _entity_t;

	private:
		component_bag<_entity_t> m_components;

	public:

	private:
	};
}