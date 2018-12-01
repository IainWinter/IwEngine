#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"
#include <tuple>

namespace iwecs {
	template<std::size_t _size_in_bytes, typename... _components_t>
	class chunk {
	public:
		using archtype_t = archtype<_components_t...>;
	private:
		using streams_t = std::tuple<_components_t*...>;
		std::size_t m_count;
		std::size_t m_capacity;
		streams_t   m_streams;

	public:
		chunk() 
		  : m_count(0),
		    m_capacity(_size_in_bytes / archtype_t::size),
		    m_streams(streams_t(new _components_t[m_capacity]...)) {}

		~chunk() {
			delete_streams();
		}

		entity_component_data insert(
			_components_t&&... components)
		{
			entity_component_data data;
			if (!is_full()) {
				data = insert_into_streams(std::forward<_components_t>(components)...);
				m_count++;
			}

			return data;
		}

		bool remove(
			std::size_t index) 
		{
			if (index > 0 && index < m_count) {
				if (index < m_count - 1) {
					remove_from_streams(index);
				}

				m_count--;
				return true;
			}

			return false;
		}

		bool is_full() {
			return m_count == m_capacity;
		}
	private:
		template<std::size_t... _tuple_index>
		entity_component_data insert_into_streams(
			std::index_sequence<_tuple_index...>,
			_components_t&&... data)
		{
			int expanded[] = {
				(std::get<_tuple_index>(m_streams)[m_count] = data, 0)...
			};

			return entity_component_data(
				sizeof...(_tuple_index),
				{ (void*)&std::get<_tuple_index>(m_streams)[m_count]... }
			);
		}

		entity_component_data insert_into_streams(
			_components_t&&... data) 
		{
			return insert_into_streams(
				std::make_index_sequence<sizeof...(_components_t)>{},
				std::forward<_components_t>(data)...
			);
		}

		template<std::size_t... _tuple_index>
		void remove_from_streams(
			std::index_sequence<_tuple_index...>,
			std::size_t remove_index) 
		{
			int expanded[] = {
				(std::get<_tuple_index>(m_streams)[remove_index]
					= std::get<_tuple_index>(m_streams)[m_count - 1], 0)...
			};
		}

		void remove_from_streams(
			std::size_t remove_index) 
		{
			remove_from_streams(
				std::make_index_sequence<sizeof...(_components_t)>{},
				remove_index
			);
		}

		template<std::size_t... _tuple_index>
		void delete_streams(
			std::index_sequence<_tuple_index...>)
		{
			int expanded[] = {
				(delete[] std::get<_tuple_index>(m_streams), 0)...
			};
		}

		void delete_streams() {
			delete_streams(
				std::make_index_sequence<sizeof...(_components_t)>{}
			);
		}
	};
}