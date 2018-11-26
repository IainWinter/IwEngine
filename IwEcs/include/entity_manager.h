#pragma once

#include "iwecs.h"
#include <unordered_map>

namespace iwecs {
	class entity_manager {
	private:
		std::unordered_map<entity_t, entity_data> m_entities;
	public:

	};
}