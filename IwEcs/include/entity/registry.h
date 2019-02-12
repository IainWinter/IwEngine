#pragma once

#include <map>
#include "component/group.h"
#include "set/sparse_set.h"

namespace iwecs {
	struct default_tag;

	template<
		typename _entity_t = std::size_t,
		typename _component_group = default_tag>
	class registry {
	public:
		using entity_type     = _entity_t;
		using component_group = _component_group;
		using group_type      = group<entity_type>;

	private:
		std::map<archetype, group_type> m_groups;
		iwutil::sparse_set<entity_type, archetype> m_entities;
		entity_type m_next_entity = 0;

	public:
		entity_type create() {
			m_entities.emplace(m_next_entity, archetype());
			return m_next_entity++;
		}

		template<
			typename _component_t,
			typename... _args_t>
		void assign(
			entity_type entity,
			_args_t&&... args)
		{
			archetype& a = m_entities.at(entity);
			if (is_empty(a)) {
				add_type<component_group, _component_t>(a);
				auto itr = m_groups.find(a);
				if (itr == m_groups.end()) {
					itr = m_groups.emplace(a, new group_type(a));
				}
			}
		}

	private:
	};
}
