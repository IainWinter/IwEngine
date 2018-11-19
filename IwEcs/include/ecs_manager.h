#pragma once

#include "iwecs.h"
#include "component_registry.h"

namespace iwecs {
	class IWECS_API ecs_manager {
	private:
		struct {
			component_registry c;
		};
	public:
		ecs_manager();
		~ecs_manager();

		template<typename... _components_t>
		entity create_entity(_components_t&&... args) {
			return c.create_entity<_components_t...>(
				std::forward<_components_t>(args)...
			);
		}
	};
}