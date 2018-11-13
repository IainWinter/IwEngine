#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "archtype.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<iwutil::id_t, iarchtype*> m_archtypes;

		template<typename... T>
		archtype<T...>& ensure_archtype() {
			archtype<T...>* a = new archtype<T...>(8);
			iwutil::id_t id = a->id();
			m_archtypes.emplace(id, a);

			return *a;
		}
	public:
		component_registry() {}
		~component_registry() {}

		template<typename... T>
		entity create_entity() {
			archtype<T...>& a = ensure_archtype<T...>();
			return a.create_entity();
		}
	};
}