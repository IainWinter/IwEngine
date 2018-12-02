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
		using archtype_t    = archtype<_components_t...>;
		using chunk_t	    = chunk<640, _components_t...>; //640 as temp value
		using entity_data_t = typename chunk_t::entity_data_t;
	private:
		std::unordered_map<std::size_t, chunk_t*> m_chunks;
		std::size_t m_working_chunk_id;
		std::size_t m_next_id;

	public:
		component_data()
		  : m_working_chunk_id(0),
			m_next_id(0) {
		}

		entity_data_t attach_components(
			_components_t&&... args) 
		{
			chunk_t& chunk = ensure_free_chunk();

			entity_data_t data = chunk.insert(std::forward<_components_t>(args)...);
			data.index += m_working_chunk_id;

			typename chunk_t::iterator itr = chunk.begin();
			typename chunk_t::iterator itr = chunk.end();


			return data;
		}

		bool destroy_components(
			index_t index) {
			index_t chunk_index = index / chunk_t::capacity;
			bool has_been_removed = m_chunks[chunk_index]->remove(index - chunk_index);

			return has_been_removed;
		}
	private:
		chunk_t& ensure_free_chunk() {
			if (no_free_chunks()) {
				return add_chunk();
			}

			chunk_t& chunk = get_working_chunk();
			if (chunk.is_full()) {
				return add_chunk();
			}

			return chunk;
		}

		chunk_t& add_chunk() {
			m_working_chunk_id = m_next_id;

			chunk_t* chunk = new chunk_t();
			m_chunks.emplace(m_next_id++, chunk);

			return *chunk;
		}

		chunk_t& get_working_chunk() {
			return *m_chunks[m_working_chunk_id];
		}

		bool no_free_chunks() {
			return m_chunks.find(m_working_chunk_id) == m_chunks.end();
		}
	};
}