#pragma once

#include <list>
#include "iwecs.h"
#include "archetype.h"
#include "array_pack.h"

namespace iwecs {
	class icomponent_list {
	public:
		virtual ~icomponent_list() {}
		virtual bool erase(std::size_t index) = 0;
	};

	template<
		typename... _components_t>
	class component_list : public icomponent_list {
	public:
		using archetype_t = iwutil::archetype<_components_t...>;
	private:
		//640 as temp value
		static constexpr std::size_t chunk_capacity = 640 / archetype_t::size_in_bytes;

		using chunk_t          = iwutil::array_pack<chunk_capacity, _components_t...>;
		using chunk_list_t     = std::list<chunk_t>;
		using chunk_list_itr_t = typename chunk_list_t::iterator;

		chunk_list_t m_chunks;
		chunk_list_itr_t m_working_chunk;
		std::size_t m_working_index;
		std::size_t m_chunk_count;

		std::size_t m_size;
		std::size_t m_capacity;

	public:
		component_list() = default;

		component_list(const component_list& copy) = delete;
		component_list(component_list&& copy) = default;

		component_list& operator=(const component_list& copy) = delete;
		component_list& operator=(component_list&& copy) = default;

		void push_pack(
			_components_t&&... args)
		{
			ensure_free_working_chunk();
			m_working_chunk->insert(std::forward<_components_t>(args)...);
			
			m_size++;

			return m_working_chunk->size()
				+ m_working_chunk->capacity()
				* m_working_index;
		}

		bool erase(
			std::size_t index)
		{
			std::size_t chunk_index     = index / chunk_t::capacity;
			std::size_t component_index = index % chunk_t::capacity;
			bool removed;
			if (chunk_index == m_working_index) {
				removed = m_working_chunk->remove(component_index);
			} else {
				removed = get_chunk(chunk_index).remove(component_index);
			}

			if (removed) {
				m_size--;
			}

			return removed;
		}

		std::size_t size() {
			return m_size;
		}

		std::size_t capacity() {
			return m_capacity;
		}
	private:
		void ensure_free_working_chunk() {
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

			m_capacity += chunk_capacity;
		}

		chunk_t& get_chunk(
			std::size_t index)
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
			std::size_t index    = m_working_index; //Check this -- Checked
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