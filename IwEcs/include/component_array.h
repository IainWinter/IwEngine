#pragma once

#include "iwecs.h"
#include <unordered_map>
#include "archtype.h"
#include "chunk.h"

namespace iwecs {
	class icomponent_array {
	public:
		virtual ~icomponent_array() {}
		virtual bool destroy_components(entity_t entity) = 0;
	};

	template<typename... _components_t>
	class component_array : public icomponent_array {
	public:
		using archtype_t = archtype<_components_t...>;
		using chunk_t = chunk<640, _components_t...>; //640 as temp value
		using component_data_t = typename chunk_t::data_t;
		using entity_data_t = typename entity_data<archtype_t::size>;
	private:
		std::list<chunk_t*> m_chunks;
		//std::unordered_map<std::size_t, chunk_t*> m_chunks;
		std::size_t m_working_chunk_index;
		std::size_t m_next_index;

	public:
		component_array()
		  : m_working_chunk_index(0),
			m_next_index(0) {}

		entity_data_t attach_components(
			_components_t&&... args)
		{
			chunk_t& chunk = ensure_free_chunk();
			component_data_t data = chunk.insert(std::forward<_components_t>(args)...);

			return entity_data_t(
				data.index + chunk_t::capacity * m_working_chunk_index,
				archtype_t::id,
				data.data
			);
		}

		bool destroy_components(
			index_t index)
		{
			index_t chunk_index = index / chunk_t::capacity;
			index_t component_index = index % chunk_t::capacity;

			return m_chunks[chunk_index]->remove(component_index);
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
			m_working_chunk_index = m_next_index;

			chunk_t* chunk = new chunk_t();
			m_chunks.push_back(chunk);
			m_next_index++;

			return *chunk;
		}

		chunk_t& get_working_chunk() {
			return *m_chunks[m_working_chunk_index];
		}

		bool no_free_chunks() {
			return m_chunks.find(m_working_chunk_index) == m_chunks.end();
		}
	};
}