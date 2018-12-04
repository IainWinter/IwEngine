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
		std::size_t m_chunk_count;
		//std::size_t m_next_index;

	public:
		entity_data_t attach_components(
			_components_t&&... args)
		{
			ensure_free_working_chunk();
			component_data_t data = get_working_chunk().insert(std::forward<_components_t>(args)...);

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
				return get_working_chunk().remove(component_index);
			}

			return get_chunk(chunk_index).remove(component_index);
		}
	private:
		void ensure_free_working_chunk() {
			if (no_chunks() || no_free_chunks() ) {
				add_chunk();
			}
		}

		void add_chunk() {
			chunk_t* chunk = new chunk_t();
			m_chunks.push_back(chunk);

			m_working_chunk = m_chunks.end()--;
			m_working_index++;
			m_chunk_count++;
		}

		chunk_t& get_working_chunk() {
			return **m_working_chunk;
		}

		chunk_t& get_chunk(
			chunk_list_itr_t itr)
		{
			return **itr;
		}

		chunk_t& get_chunk(
			index_t index)
		{
			auto itr = m_chunks.begin();
			std::advance(itr, index);
			
			return **itr;
		}

		bool no_chunks() {
			return m_chunk_count == 0;
		}

		bool no_free_chunks() {
			if (!get_working_chunk().is_full()) {
				return false;
			}

			bool has_found = false;
			chunk_list_itr_t found_itr;
			chunk_list_itr_t itr = m_working_chunk;
			std::size_t index    = m_working_index - 1;
			while (index > 0) {
				itr--;
				index--;
				if (!get_chunk(itr).is_full()) {
					found_itr = itr;
					has_found = true;
				}
			}

			if (has_found) {
				m_working_chunk = found_itr;
				m_working_index = index;
				return false;
			}

			itr = m_working_chunk;
			while (index > 0) {
				itr++;
				index++;
				if (!get_chunk(itr).is_full()) {
					found_itr = itr;
					has_found = true;
				}
			}

			if (has_found) {
				m_working_chunk = found_itr;
				m_working_index = index;
				return false;
			}

			return true;
		}
	};
}