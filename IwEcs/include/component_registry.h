#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "component_data.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<iwutil::id_t, icomponent_data*> m_cdata;

		template<typename... _components_t>
		component_data<_components_t...>& ensure_cdata() {
			using cdata_t = component_data<_components_t...>;

			cdata_t* cdata_ptr;
			iwutil::id_t id = cdata_t::archtype_t::id();
			if (m_cdata.find(id) == m_cdata.end()) {
				cdata_ptr = new cdata_t();
				m_cdata.emplace(id, cdata_ptr);
			} else {
				cdata_ptr = (cdata_t*)m_cdata[id];
			}

			return *cdata_ptr;
		}
	public:
		template<typename... _components_t>
		entity create_entity(_components_t&&... args) {
			using cdata_t = component_data<_components_t...>;

			cdata_t& cdata = ensure_cdata<_components_t...>();
			return cdata.create_entity(
				std::forward<_components_t>(args)...
			);
		}
	};
}