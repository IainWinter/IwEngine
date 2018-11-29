#pragma once

#include "iwecs.h"
#include <unordered_map>
#include "archtype.h"
#include "chunk.h"

namespace iwecs {
	class icomponent_data {
	public:
		virtual ~icomponent_data() {}
		virtual bool destroy_components(entity_t entity) = 0;
	};

	template<typename... _components_t>
	class component_data : public icomponent_data {
	public:
		using archtype_t = archtype<_components_t...>;
		using chunk_t	 = chunk<640, _components_t...>; //640 as temp value
	private:
		std::unordered_map<std::size_t, chunk_t*> m_chunks;
		std::size_t m_working_chunk_id;
		std::size_t m_next_id;

		void add_chunk() {
			m_working_chunk_id = m_next_id;
 			m_chunks.emplace(m_next_id++, new chunk_t());
		}

		chunk_t& ensure_free_chunk() {
			if (m_chunks.find(m_working_chunk_id) == m_chunks.end()) {
				chunk_t& chunk = &m_chunks[m_working_chunk_id];
				if (chunk->is_full()) {
					add_chunk();
				}

				return chunk;
			}

			return &m_chunks[m_working_chunk_id];
		}
	public:
		component_data()
		  : m_working_chunk_id(0),
			m_next_id(0) {}

		entity_data attach_components(_components_t&&... args) {
			chunk_t& chunk = ensure_free_chunk();
			entity_component_data data = chunk.insert(std::forward<_components_t>(args)...);
			return entity_data(0, data);
		}

		bool destroy_components(entity_t entity) {
			//get id from entity_data 
			return false;
		}
	};
}