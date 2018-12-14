#pragma once

#include <unordered_map>
#include "iwecs.h"
#include "component_array.h"
#include "type_id.h"

namespace iwecs {
	class component_registry {
	private:
		std::unordered_map<iwutil::type_id_t, icomponent_array*> m_carray;

	public:
		template<
			typename... _components_t>
		ientity_data create_entity(
			_components_t&&... args)
		{
			using carray_t = component_array<_components_t...>;

			carray_t& carray = ensure_carray<_components_t...>();
			return carray.attach_components(
				std::forward<_components_t>(args)...
			);
		}

		bool destroy_entity(
			index_t index,
			iwutil::type_id_t archetype_id)
		{
			if (no_carray(archetype_id)) {
				return false;
			}

			return m_carray.at(archetype_id)->destroy_components(index);
		}

		template<
			typename... _components_t>
		/* component_view */ void view_components() {

		}
	private:
		template<typename... _components_t>
		component_array<_components_t...>& ensure_carray() {
			using carray_t = component_array<_components_t...>;

			if (no_carray(carray_t::archetype_t::id)) {
				return add_carray<_components_t...>();
			}

			return get_carray<_components_t...>();
		}

		template<
			typename... _components_t>
		component_array<_components_t...>& add_carray() {
			using carray_t = component_array<_components_t...>;

			carray_t* carray = new carray_t();
			m_carray.emplace(carray_t::archetype_t::id, carray);

			return *carray;
		}

		template<
			typename... _components_t>
		component_array<_components_t...>& get_carray() {
			using carray_t = component_array<_components_t...>;

			return (component_array<_components_t...>&)
				*m_carray[carray_t::archetype_t::id];
		}

		template<
			typename... _components_t>
		bool no_carray(
			iwutil::type_id_t id) 
		{
			return m_carray.find(id) == m_carray.end();
		}
	};
}