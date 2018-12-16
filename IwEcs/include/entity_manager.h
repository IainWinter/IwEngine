#pragma once

#include "iwecs.h"
#include <unordered_map>

namespace iwecs {
	using entity_t = std::size_t;

	struct entity_data {
		std::size_t archetype_id;
		std::size_t component_index;
	};

	class IWECS_API entity_manager {
	private:
		struct {
			std::unordered_map<entity_t, entity_data> m_entities;
			entity_t m_next_id;
		};
	public:
		entity_manager();

		entity_t add_entity(
			std::size_t archetype_id,
			std::size_t component_index);

		entity_data& get_entity_data(
			entity_t entity);
	};
}