#pragma once

#include "iwecs.h"
#include <unordered_map>

namespace iwecs {
	class entity_manager {
	private:
		std::unordered_map<entity_t, ientity_data> m_entities;
		entity_t m_next_id;
	public:
		entity_manager()
			: m_next_id(0) {}

		entity_t add_entity(const ientity_data& data) {
			m_entities.emplace(++m_next_id, data);
			return m_next_id;
		}

		ientity_data& get_entity_data(entity_t entity) {
			return m_entities.at(entity);
		}
	};
}