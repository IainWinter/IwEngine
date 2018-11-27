#pragma once

#include "iwecs.h"
#include "component_registry.h"
#include "entity_manager.h"

namespace iwecs {
	class IWECS_API ecs_manager {
	private:
		struct {
			component_registry component_reg;
			entity_manager entity_mgr;
		};
	public:
		ecs_manager();
		~ecs_manager();

		template<typename... _components_t>
		entity_t create_entity(_components_t&&... args) {
			entity_data data = component_reg.create_entity<_components_t...>(
				std::forward<_components_t>(args)...
			);

			return entity_mgr.add_entity(data);
		}

		void destroy_entity(entity_t entity) {
			component_reg.destroy_entity(entity);
		}
	};
}