#pragma once

#include "input_iterator_pack.h"
#include "archetype.h"
#include "tuple_iteration.h"
#include <tuple>
#include <array>

namespace iwutil {
	template<
		std::size_t _size_in_bytes,
		typename... _t>
	class array_pack {
	public:
		using archetype_t    = archetype<_t...>;
		using iterator       = iwutil::input_iterator_pack<_t...>;
		using const_iterator = iwutil::input_iterator_pack<const _t...>;
	private:
		using arrays_t = std::tuple<_t*...>;

		static constexpr std::size_t m_capacity
			= _size_in_bytes / archetype_t::size_in_bytes;

		std::size_t m_size;
		arrays_t   m_arrays;

	public:
		array_pack() 
		  : m_size(0),
			m_arrays(arrays_t(new _t[m_capacity]...)) {}

		array_pack(
			const array_pack& copy)
		  : m_size(copy.m_size),
			m_arrays(arrays_t(new _t[m_capacity]...)) 
		{
			copy_streams(copy.m_arrays);
		}

		array_pack(
			array_pack&& copy)
		  : m_size(copy.m_size),
			m_arrays(arrays_t(new _t[m_capacity]...)) 
		{
			copy_streams(copy.m_arrays);
		}

		~array_pack() {
			delete_streams();
		}

		array_pack& operator=(
			const array_pack& copy)
		{
			m_size = copy.m_size;
			m_arrays = arrays_t(new _t[m_capacity]...);
			copy_streams(copy.m_arrays);
		}

		array_pack& operator=(
			array_pack&& copy)
		{
			m_size = copy.m_size;
			m_arrays = arrays_t(new _t[m_capacity]...);
			copy_streams(copy.m_arrays);
		}

		void insert(
			_t&&... components)
		{
			if (!is_full()) {
				data = insert_into_streams(std::forward<_t>(components)...);
				m_size++;
			}
		}

		bool remove(
			std::size_t index) 
		{
			if (index >= 0 && index < m_size) {
				if (index <= m_size - 1) {
					remove_from_streams(index);
				}

				m_size--;
				return true;
			}

			return false;
		}

		bool is_full() {
			return m_size == m_capacity;
		}

		std::size_t size() {
			return m_size;
		}

		std::size_t capacity() {
			return m_capacity;
		}

		iterator begin() {
			return get_iterator();
		}

		iterator end() {
			return get_iterator_end();
		}
	private:
		struct collapse_array {
			template<
				typename _t>
			void operator()(
				_t&& stream,
				std::size_t index,
				std::size_t end_index)
			{
				stream[index] = stream[end_index];
			}
		};

		struct insert_array {
			template<
				typename _t,
				typename _d>
			void operator()(
				_t&& stream,
				_d&& data,
				std::size_t index)
			{
				stream[index] = data;
			}
		};

		struct copy_array {
			template<
				typename _t,
				typename _d>
			void operator()(
				_t&& stream,
				_d&& copy,
				std::size_t count)
			{
				memcpy(stream, copy, count * sizeof(_t));
			}
		};

		void insert_into_streams(
			_t&&... data) 
		{
			iwutil::foreach<
				insert_array,
				arrays_t,
				std::tuple<_t&&...>,
				archetype_t::size>
			(
				m_arrays,
				std::forward_as_tuple<_t...>(std::forward<_t>(data)...),
				m_size
			);
		}

		void remove_from_streams(
			std::size_t remove_index) 
		{
			iwutil::foreach<
				collapse_array,
				arrays_t,
				archetype_t::size>
			(
				m_arrays,
				remove_index,
				m_size - 1
			);
		}

		void delete_streams() {
			iwutil::foreach<
				functors::erase_array,
				arrays_t,
				archetype_t::size>
			(
				m_arrays
			);
		}

		void copy_streams(
			const arrays_t& copy)
		{
			iwutil::foreach<
				copy_array,
				arrays_t,
				arrays_t,
				archetype_t::size>
			(
				m_arrays,
				copy,
				m_capacity
			);
		}

		iterator get_iterator() {
			return iwutil::geteach<
				functors::index,
				arrays_t,
				iterator,
				archetype_t::size>
			(
				m_arrays,
				0
			);
		}

		iterator get_iterator_end() {
			return iwutil::geteach<
				functors::index,
				arrays_t,
				iterator,
				archetype_t::size>
			(
				m_arrays,
				m_size
			);
		}
	};
}