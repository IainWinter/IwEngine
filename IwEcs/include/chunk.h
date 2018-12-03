#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"
#include <tuple>

namespace iwecs {
	template<std::size_t _size>
	struct chunk_data {
		std::size_t index;
		void* data[_size];

		chunk_data()
		  : index(0) {}

		chunk_data(
			index_t index_,
			void* data_[_size])
		  : index(index_)
		{
			memcpy(data, data_, _size * sizeof(void*));
		}
	};

	template<std::size_t _size_in_bytes, typename... _t>
	class chunk {
	public:
		using archtype_t = archtype<_t...>;
		using data_t     = chunk_data<archtype_t::size>;

		static constexpr std::size_t capacity = _size_in_bytes / archtype_t::size_in_bytes;
	private:
		using streams_t = std::tuple<_t*...>;

		std::size_t m_count;
		streams_t   m_streams;

	public:
		chunk() 
		  : m_count(0),
			m_streams(streams_t(new _t[capacity]...)) {}

		chunk(const chunk& copy) = delete;
		chunk(chunk&& copy) = delete;

		~chunk() {
			delete_streams();
		}

		chunk& operator=(const chunk& copy) = delete;
		chunk& operator=(chunk&& copy) = delete;

		data_t insert(
			_t&&... components)
		{
			data_t data;
			if (!is_full()) {
				data = insert_into_streams(std::forward<_t>(components)...);
				m_count++;
			}

			return data;
		}

		bool remove(
			std::size_t index) 
		{
			if (index >= 0 && index < m_count) {
				if (index <= m_count - 1) {
					remove_from_streams(index);
				}

				m_count--;
				return true;
			}

			return false;
		}

		bool is_full() {
			return m_count == capacity;
		}

		//iterator begin() {
		//	return get_iterator();
		//}

		//iterator end() {
		//	return get_iterator_end();
		//}
	private:
		template<std::size_t... _tuple_index>
		data_t insert_into_streams(
			std::index_sequence<_tuple_index...>,
			_t&&... data)
		{
			int expanded[] = {
				(std::get<_tuple_index>(m_streams)[m_count] = data, 0)...
			};

			void* components[archtype_t::size] = {
				(void*)&std::get<_tuple_index>(m_streams)[m_count]...
			};

			return data_t(m_count, components);
		}

		data_t insert_into_streams(
			_t&&... data) 
		{
			return insert_into_streams(
				std::make_index_sequence<sizeof...(_t)>{},
				std::forward<_t>(data)...
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
				std::make_index_sequence<archtype_t::size>{},
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
			delete_streams(std::make_index_sequence<archtype_t::size>{});
		}

		//template<std::size_t... _tuple_index>
		//iterator get_iterator(
		//	std::index_sequence<_tuple_index...>,
		//	index_t index)
		//{
		//	return iterator(std::get<_tuple_index>(m_streams) + index...);
		//}

		//iterator get_iterator() {
		//	return get_iterator(std::make_index_sequence<archtype_t::size>{}, 0);
		//}

		//iterator get_iterator_end() {
		//	return get_iterator(std::make_index_sequence<archtype_t::size>{}, m_count - 1);
		//}
	};
}