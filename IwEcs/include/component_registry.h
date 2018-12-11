#pragma once

#include <unordered_map>
#include "iwecs.h"
#include "component_array.h"
#include "IwUtil/type_id.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<iwutil::type_id_t, icomponent_array*> m_cdata;

	public:
		template<
			typename... _components_t>
		ientity_data create_entity(
			_components_t&&... args)
		{
			using cdata_t = component_array<_components_t...>;

			cdata_t& cdata = ensure_cdata<_components_t...>();
			return cdata.attach_components(
				std::forward<_components_t>(args)...
			);
		}

		bool destroy_entity(
			index_t index,
			iwutil::type_id_t archetype_id)
		{
			if (no_cdata(archetype_id)) {
				return false;
			}

			return m_cdata.at(archetype_id)->destroy_components(index);
		}

		template<
			typename... _components_t>
		/* component_view */ void view_components() {

		}
	private:
		template<typename... _components_t>
		component_array<_components_t...>& ensure_cdata() {
			using cdata_t = component_array<_components_t...>;

			if (no_cdata(cdata_t::archtype_t::id)) {
				return add_cdata<_components_t...>();
			}

			return get_cdata<_components_t...>();
		}

		template<
			typename... _components_t>
		component_array<_components_t...>& add_cdata() {
			using cdata_t = component_array<_components_t...>;

			cdata_t* cdata = new cdata_t();
			m_cdata.emplace(cdata_t::archtype_t::id, cdata);

			return *cdata;
		}

		template<
			typename... _components_t>
		component_array<_components_t...>& get_cdata() {
			using cdata_t = component_array<_components_t...>;

			return (component_array<_components_t...>&)
				*m_cdata[cdata_t::archtype_t::id];
		}

		template<
			typename... _components_t>
		bool no_cdata(
			iwutil::type_id_t id) 
		{
			return m_cdata.find(id) == m_cdata.end();
		}
	};
}