#pragma once

#include <unordered_map>
#include <typeinfo>
#include "iwecs.h"
#include "component_data.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<std::size_t, icomponent_data*> m_cdata;

	public:
		template<typename... _components_t>
		ientity_data create_entity(
			_components_t&&... args) 
		{
			using cdata_t = component_data<_components_t...>;

			cdata_t& cdata = ensure_cdata<_components_t...>();
			return cdata.attach_components(
				std::forward<_components_t>(args)...
			);
		}

		bool destroy_entity(
			index_t index) 
		{
			//Way better way to do this but it works for now
			for (auto& cdata : m_cdata) {
				bool destroyed = cdata.second->destroy_components(index);
				if  (destroyed) return true;
			}

			return false;
		}
	private:
		template<typename... _components_t>
		component_data<_components_t...>& ensure_cdata() {
			std::size_t id = typeid(archtype<_components_t...>).hash_code(); //use archtype somehow
			if (no_cdata(id)) {
				return add_cdata<_components_t...>(id);
			}	

			return get_cdata<_components_t...>(id);
		}

		template<typename... _components_t>
		component_data<_components_t...>& add_cdata(index_t index)
		{
			using cdata_t = component_data<_components_t...>;

			cdata_t* cdata = new cdata_t();
			m_cdata.emplace(index, cdata);

			return *cdata;
		}

		template<typename... _components_t>
		component_data<_components_t...>& get_cdata(index_t index) {
			return (component_data<_components_t...>&)*m_cdata[index];
		}

		bool no_cdata(index_t index) {
			return m_cdata.find(index) == m_cdata.end();
		}
	};
}