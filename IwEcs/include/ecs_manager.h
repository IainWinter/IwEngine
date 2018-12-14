#pragma once

#include "iwecs.h"
#include "component_registry.h"
#include "entity_manager.h"

namespace iwecs {
	class IWECS_API ecs_manager {
	private:
		struct {
			component_registry m_component_reg;
			entity_manager m_entity_mgr;
		};
	public:
		ecs_manager();
		~ecs_manager();

		template<typename... _components_t>
		entity_t create_entity(
			_components_t&&... args) 
		{
			ientity_data data = m_component_reg.create_entity<_components_t...>(
				std::forward<_components_t>(args)...
			);

			return m_entity_mgr.add_entity(data);
		}

		template<typename... _components_t>
		entity_t create_entity(
			_components_t&... args)
		{
			return create_entity(std::forward<_components_t>(args)...);
		}

		void destroy_entity(
			entity_t entity) 
		{
			ientity_data data = m_entity_mgr.get_entity_data(entity);
			m_component_reg.destroy_entity(data.index, data.archetype_id);
		}

		template<typename... _components_t>
		/* component_view */ void view_components() {
			return m_component_reg.view_components<_components_t...>();
		}
	};
}