#pragma once

#include <list>
#include "IwUtil/archtype.h"
#include "iwecs.h"
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
		using archtype_t       = iwutil::archtype<_components_t...>;
		using chunk_t          = chunk<640, _components_t...>; //640 as temp value
		using iterator         = typename std::list<chunk_t>::iterator;
	private:
		using component_data_t = typename chunk_t::data_t;
		using entity_data_t    = typename entity_data<archtype_t::size>;
		using chunk_list_t     = std::list<chunk_t>;

		chunk_list_t m_chunks;
		iterator m_working_chunk;
		std::size_t m_working_index;
		std::size_t m_chunk_count;

	public:
		entity_data_t attach_components(
			_components_t&&... args)
		{
			ensure_free_working_chunk();
			component_data_t data = m_working_chunk->insert(
				std::forward<_components_t>(args)...);

			for (iterator i = m_chunks.begin(); i != m_chunks.end(); i++) {
				for (typename chunk_t::iterator j = i->begin(); j != i->end(); j++) {
					;
				}
			}

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

		iterator begin() {

		}
	private:
		void ensure_free_working_chunk(){
			if (no_chunks() || find_free_chunk()) {
				add_chunk();
			}
		}

		void add_chunk() {
			m_chunks.push_back(chunk_t());

			m_working_chunk = m_chunks.end();
			m_working_chunk--;

			m_chunk_count++;
			m_working_index = m_chunk_count - 1;
		}

		chunk_t& get_chunk(
			index_t index)
		{
			auto itr = m_chunks.begin();
			std::advance(itr, index);
			
			return *itr;
		}

		bool no_chunks() {
			return m_chunk_count == 0;
		}

		bool find_free_chunk() {
			if (!m_working_chunk->is_full()) {
				return false;
			}

			bool has_found = false;
			iterator found_itr;
			iterator itr = m_working_chunk;
			std::size_t index = m_working_index;
			while (index > 0) {
				itr--;
				index--;
				if (!itr->is_full()) {
					found_itr = itr;
					has_found = true;
				}
			}

			if (has_found) {
				m_working_chunk = found_itr;
				m_working_index = index;
				return false;
			}

			itr   = m_working_chunk;
			index = m_working_index;
			while (index < m_chunk_count - 1) {
				itr++;
				index++;
				if (!itr->is_full()) {
					m_working_chunk = itr;
					m_working_index = index;

					return false;
				}
			}

			return true;
		}
	};
}