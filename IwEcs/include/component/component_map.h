#pragma once

#include <unordered_map>
#include "component_group.h"
#include "type_id.h"

namespace iwecs {
	class component_map {
	private:
		using id_t = iwutil::typeid_t;

		std::unordered_multimap<id_t, icomponent_group*> _groups;

	public:
		template<
			typename _component_t>
		void emplace(
			unsigned int entity,
			_component_t&& component)
		{
			using group_t = component_group<_component_t>;

			id_t id = iwutil::type_id<_component_t>();
			auto itr = _groups.find(id);
			if (itr == _groups.end()) {
				itr = _groups.emplace(id, new group_t());
			}

			group_t* group = reinterpret_cast<group_t*>(itr->second);
			group->insert(std::forward<_component_t>(component));
		}

	private:
	};
}