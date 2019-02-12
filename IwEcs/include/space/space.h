#pragma once

#include <unordered_map>
#include "common_space.h"
#include "sub_space.h"
#include "component/archetype.h"

namespace iwent {
	class space {
	public:
		using entity_type = size_t;
	private:
		std::vector<archetype>   m_entities;
		std::vector<entity_type> m_expired;
		entity_type m_next_entity;

		std::vector<sub_space> m_subspaces;
		common_space m_common_space;
		//list of subspaces

	public:
		//Creates an empty entity
		entity_type create() {
			return create(archetype());
		}

		//Creates an entity conforming to an archetype
		entity_type create(archetype a) {
			if (m_expired.size() != 0) {
				entity_type entity = m_expired.back();
				m_expired.pop_back();
				m_entities[entity] = a;
				return entity;
			} else {
				m_entities.push_back(a);
				return m_next_entity++;
			}
		}

		//Assigns additional components onto 
		template<
			typename _component_t,
			typename... _args_t>
		void assign(entity_type entity, _args_t&&... args);

		//Destroys components in archetype
		void destroy(entity_type entity, archetype a) {
			for (auto sub : m_subspaces) {
				if (sub.is_similar(a)) {
					//Remove component from subspace
				}
			}
		}

		//Destroys all components assigned to the entity
		void destroy(entity_type entity) {
			m_expired.push_back(entity);
			destroy(entity, m_entities[entity]);
		}

		bool divide(archetype a) {
			for (auto sub : m_subspaces) {
				if (sub.is_similar(a)) {
					return false;
				}
			}

			m_subspaces.push_back(sub_space(a));

			return true;
		}
	};
}
