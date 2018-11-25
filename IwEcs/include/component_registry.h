#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "component_data.h"

namespace iwecs {
	class component_registry {
	private:
		entity m_next_entity;
		std::unordered_map<std::size_t, icomponent_data*> m_cdata;

		template<typename... _components_t>
		component_data<_components_t...>& ensure_cdata() {
			using cdata_t = component_data<_components_t...>;

			cdata_t* cdata_ptr;
			std::size_t id = typeid(cdata_t).hash_code();
			if (m_cdata.find(id) == m_cdata.end()) {
				cdata_ptr = new cdata_t();
				m_cdata.emplace(id, cdata_ptr);
			} else {
				cdata_ptr = (cdata_t*)m_cdata[id];
			}

			return *cdata_ptr;
		}
	public:
		component_registry() : m_next_entity(1) {}

		template<typename... _components_t>
		entity create_entity(_components_t&&... args) {
			using cdata_t = component_data<_components_t...>;

			cdata_t& cdata = ensure_cdata<_components_t...>();
			cdata.attach_components(
				m_next_entity,
				std::forward<_components_t>(args)...
			);

			return m_next_entity++;
		}

		template<typename... _components_t>
		entity create_entity2(_components_t&... args) {
			using cdata_t = component_data<_components_t...>;

			cdata_t& cdata = ensure_cdata<_components_t...>();
			cdata.attach_components(
				m_next_entity,
				std::forward<_components_t>(args)...
			);

			return m_next_entity++;
		}

		bool destroy_entity(entity entity) {
			for (auto& cdata : m_cdata) {
				cdata.second->destroy_components(entity);
			}

			return false;
		}
	};
}