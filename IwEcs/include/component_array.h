#pragma once

#include <list>
#include "archetype.h"
#include "iwecs.h"
#include "array_pack.h"

namespace iwecs {
	class icomponent_array {
	public:
		virtual ~icomponent_array() {}
		virtual bool destroy_components(entity_t entity) = 0;
	};

	template<
		typename... _components_t>
	class component_array : public icomponent_array {
	public:
		using archetype_t      = iwutil::archetype<_components_t...>;
	private:
		//640 as temp value
		using chunk_t          = iwutil::array_pack<640, _components_t...>;
		using chunk_list_t     = std::list<chunk_t>;
		using chunk_list_itr_t = typename chunk_list_t::iterator;

		chunk_list_t m_chunks;
		chunk_list_itr_t m_working_chunk;
		std::size_t m_working_index;
		std::size_t m_chunk_count;

	public:
		entity_t attach_components(
			_components_t&&... args)
		{
			ensure_free_working_chunk();
			m_working_chunk->insert(std::forward<_components_t>(args)...);
			
			return m_working_chunk->size() 
				+ m_working_chunk->capacity()
				* m_working_index;
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
			chunk_list_itr_t found_itr;
			chunk_list_itr_t itr = m_working_chunk;
			std::size_t index = m_working_index; //Check this
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