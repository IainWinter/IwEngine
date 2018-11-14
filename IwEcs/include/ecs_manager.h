#pragma once

#include "iwecs.h"
#include "component_registry.h"

namespace iwecs {
	class IWECS_API ecs_manager {
	private:
		component_registry c;
	public:
		ecs_manager();
		~ecs_manager();

		template<typename... _ts>
		entity create_entity() {
			return c.create_entity<_ts...>();
		}
	};
}