#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "component_data.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<iwutil::id_t, icomponent_data*> m_cdata;

		template<typename... _ts>
		icomponent_data& ensure_component_data() {
			using cdata_t = component_data<_ts...>;

			iwutil::id_t id = cdata_t::archtype_t::id();
			if (m_cdata.find(id) == m_cdata.end()) {
				m_cdata.emplace(id, new cdata_t());
			}

			return *m_cdata[id];
		}
	public:
		template<typename... _ts>
		entity create_entity() {
			icomponent_data& cdata = ensure_component_data<_ts...>();
			return cdata.create_entity();
		}
	};
}