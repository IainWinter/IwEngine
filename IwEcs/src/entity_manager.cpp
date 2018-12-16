#include "entity_manager.h"

namespace iwecs {
	entity_manager::entity_manager()
		:m_next_id(0) {}

	entity_t entity_manager::add_entity(
		std::size_t archetype_id,
		std::size_t component_index)
	{
		m_entities.emplace(
			++m_next_id, entity_data{ archetype_id, component_index });

		return m_next_id;
	}

	entity_data& entity_manager::get_entity_data(entity_t entity) {
		return m_entities.at(entity);
	}
}