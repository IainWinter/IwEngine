#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"
#include "IwUtil/bimap.h"
#include <tuple>

template<std::size_t...>
struct add_all
	: std::integral_constant<std::size_t, 0>
{};

template<std::size_t n, std::size_t... ns>
struct add_all<n, ns...>
	: std::integral_constant<std::size_t, n + add_all<ns...>::value>
{};

template<typename... _t>
std::size_t sizeof_sum() {
	return add_all<sizeof(_t)...>::value;
}

namespace iwecs {
	template<typename... _components>
	class chunk {
	private:
		using streams_t = std::tuple<_components*...>;

		streams_t   m_streams;
		std::size_t m_size;
		std::size_t m_capacity;
		std::size_t m_count;

		template<std::size_t... _tuple_index>
		void insert_into_streams(std::index_sequence<_tuple_index...>, _components&&... data) {
			int expanded[] = {(
				std::get<_tuple_index>(m_streams)[m_count] = data, 0)... 
			};
		}

		void insert_into_streams(_components&&... data) {
			insert_into_streams(std::make_index_sequence<sizeof...(_components)>{}, std::forward<_components>(data)...);
		}

		template<std::size_t... _tuple_index>
		void remove_from_streams(std::index_sequence<_tuple_index...>, std::size_t remove_index) {
			int expanded[] = {(
				std::get<_tuple_index>(m_streams)[remove_index] = std::get<_tuple_index>(m_streams)[m_count - 1], 0)...
			};
		}

		void remove_from_streams(std::size_t remove_index) {
			remove_from_streams(std::make_index_sequence<sizeof...(_components)>{}, remove_index);
		}
	public:
		chunk(std::size_t prefered_size) {
			size_t size_pre_entitiy = sizeof_sum<_components...>();
			m_size = prefered_size - prefered_size % size_pre_entitiy;
			m_capacity = m_size / size_pre_entitiy;
			m_streams = streams_t(new _components[m_capacity]...);
			m_count = 0;
		}

		void insert(_components&&... data) {
			if (!is_full()) {
				insert_into_streams(std::forward<_components>(data)...);
				m_count++;
			}
		}

		bool remove(std::size_t index) {
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
	};
}

namespace iwecs_o {
	//Find better name for this
	template<typename _t>
	class chunk {
	private:
		std::size_t m_size;
		std::size_t m_capacity;
		iwutil::bimap<std::size_t, _t*> m_index;
		_t* m_start;
		_t* m_end;
		_t* m_pointer;

		void init() {
			m_start = new _t[m_capacity];
			m_end = m_start + m_capacity;
			m_pointer = m_start;
		}
	public:
		using iterator = iwutil::input_iterator<_t>;

		chunk(std::size_t pref_size)
			: m_size(pref_size - pref_size % sizeof(_t)),
			m_capacity(m_size / sizeof(_t))
		{
			init();
		}

		chunk(const chunk& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_index(copy.m_index)
		{
			init();
			memcpy(m_start, copy.m_start, m_size);
		}

		chunk(chunk&& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_index(copy.m_index)
		{
			init();
			memcpy(m_start, copy.m_start, m_size);
		}

		~chunk() {
			delete[] m_start;
		}

		chunk& operator=(const chunk& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			m_index = copy.m_index;
			init();
			memcpy(m_start, copy.m_start, m_size);
		}

		chunk& operator=(chunk&& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			m_index = copy.m_index;
			init();
			memcpy(m_start, copy.m_start, m_size);
		}

		void insert(std::size_t key, _t&& data) {
			if (m_pointer != m_end) {
				*m_pointer = data;
				m_index.emplace(key, m_pointer);

				while (m_index.at(m_pointer)) {
					m_pointer++;
				}
			}
		}

		bool remove(std::size_t key) {
			auto itr = m_index.find(key);
			bool is_found = itr != m_index.end();
			if (is_found) {
				_t* index_ptr = nullptr;// itr->second;
				if (m_pointer > index_ptr) {
					m_pointer = index_ptr;
				}

				m_index.erase(key);
			}

			return is_found;
		}

		//Doesn't actually have to be correct
		bool is_full() {
			return m_pointer == m_end;
		}

		iterator begin() {
			return iterator(m_start);
		}

		iterator end() {
			return iterator(m_end);
		}
	};
}