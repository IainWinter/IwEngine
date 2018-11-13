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
		iarchtype& ensure_archtype() {
			iwutil::id_t id = archtype<T...>::id();
			if (m_archtypes.find(id) == m_archtypes.end()) {
				m_archtypes.emplace(id, new archtype<T...>(8));
			}

			return *m_archtypes[id];
		}
	public:
		component_registry() {}
		~component_registry() {}

		template<typename... T>
		entity create_entity() {
			iarchtype& a = ensure_archtype<T...>();
			return a.create_entity();
		}
	};
}