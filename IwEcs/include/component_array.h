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
		using archtype_t       = archtype<_components_t...>;
		using chunk_t          = chunk<640, _components_t...>; //640 as temp value
		using component_data_t = typename chunk_t::data_t;
		using entity_data_t    = typename entity_data<archtype_t::size>;
		using chunk_list_t     = std::list<chunk_t*>;
		using chunk_list_itr_t = typename chunk_list_t::iterator;
	private:
		chunk_list_t m_chunks;
		chunk_list_itr_t m_working_chunk;
		std::size_t m_working_index;
		//std::size_t m_next_index;

	public:
	entity_data_t attach_components(
			_components_t&&... args)
		{
			ensure_free_working_chunk();
			component_data_t data = chunk.insert(std::forward<_components_t>(args)...);

			return entity_data_t(
				data.index + chunk_t::capacity * m_working_index,
				archtype_t::id,
				data.data
			);
		}

		bool destroy_components(
			index_t index)
		{
			index_t chunk_index = index / chunk_t::capacity;
			index_t component_index = index % chunk_t::capacity;
			if (chunk_index == m_working_index) {
				return m_working_chunk->remove(component_index);
			}

			return get_chunk(chunk_index).remove(component_index);
		}
	private:
		void ensure_free_working_chunk() {
			//If no chunks
			// set new chunk
			//Else
			//	get_first_free_chunk
			

			if (no_free_chunks()) {
				return add_chunk();
			}

			chunk_t& chunk = get_first_free_chunk();
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
			return *m_working_chunk;
		}

		chunk_t& get_chunk(
			index_t index)
		{
			auto& itr = m_chunks.begin();
			std::advance(itr, index);
			
			return *itr;
		}

		bool no_free_chunks() {
			return m_working_chunk == m_chunks.end();
		}
	};
}