#pragma once

#include <vector>
#include "archetype.h"
#include "set/sparse_set.h"

namespace iwecs {
	template<
		typename _entity_t>
	class group {
	public:
		using entity_type = _entity_t;
	private:
		template<typename _t>
		using set_t = iwutil::sparse_set<entity_type, _t>;

		archetype m_archetype;
		iwutil::sparse_set<size_t, iwutil::sparse_set<entity_type>*> m_components;

	public:
		group() = default;

		group(archetype archetype)
			: m_archetype(archetype)
		{}

		template<
			typename _t,
			typename... _args_t>
		void create(
			entity_type entity,
			_args_t&&... args)
		{
			if (m_archetype.contains_type<_t>()) {
				ensure_set<_t>().emplace(entity, std::forward<_args_t>(args)...);
			}
		}
	private:
		template<
			typename _t>
		set_t<_t>& ensure_set() {
			int index = m_archetype.id<_t>();
			if (!m_components.contains(index)) {
				m_components.emplace(index, new set_t<_t>());
			}

			return static_cast<set_t<_t>&>(*m_components.at(index));
		}
	};
}