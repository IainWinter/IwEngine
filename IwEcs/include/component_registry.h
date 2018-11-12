#pragma once

#include "iwecs.h"
#include <vector>
#include "archtype.h"

namespace iwecs {
	class component_registry {
	private:
		std::vector<iarchtype*> m_archtypes;
	public:
		component_registry() {}
		~component_registry() {}

		template<typename... T>
		void add_archtype() {
			m_archtypes.push_back(new archtype<T...>(8));
		}

		template<typename... T>
		entity add_entity() {

		}
	};
}