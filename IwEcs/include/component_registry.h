#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "archtype.h"

namespace iwecs {
	class component_registry {
	private:
		using typeid_t = unsigned int;

		std::unordered_map<typeid_t, iarchtype*> m_archtypes;
	public:
		component_registry() {}
		~component_registry() {}

		template<typename... T>
		void add_archtype() {
			iarchtype* a = new archtype<T...>(8);
			typeid_t id = a.id();
			m_archtypes.emplace(id, a);
		}

		template<typename... T>
		entity add_entity() {
			return 0;
		}
	};
}