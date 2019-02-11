#pragma once

#include <map>
#include "component/group.h"
#include "set/sparse_set.h"

namespace iwecs {
	template<
		typename _entity_t = std::size_t>
	class registry {
	public:
		using entity_type = _entity_t;
		using group_type = group<entity_type>;

	private:
		std::map<archetype, group_type> m_groups;
		iwutil::sparse_set<entity_type, archetype> m_entities;
		entity_type m_next_entity = 0;

	public:
		entity_type create() {
			m_entities.emplace(m_next_entity, iwecs::make_archetype<>());
			return m_next_entity++;
		}

		template<
			typename _t,
			typename... _args_t>
		void assign(
			entity_type entity,
			_args_t&&... args)
		{
			archetype& a = m_entities.at(entity);
			auto itr = m_groups.find(a);
			if (itr != m_groups.end()) {
				group_type g = itr->second;
				
			}
			
			else {
				a.add_type<_t>();
				itr = m_groups.emplace(a, group_type(a)).first;
				itr->second.create<_t>(std::forward<_args_t>(args)...);
			}
		}

	private:
	};
}