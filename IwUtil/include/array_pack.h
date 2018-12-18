#pragma once

#include <tuple>
#include <array>
#include "archetype.h"
#include "tuple_iteration.h"
#include "input_iterator_pack.h"

namespace iwutil {
	template<
		std::size_t _capacity,
		typename... _t>
	class array_pack {
	public:
		using archetype_t    = archetype<_t...>;
		using iterator       = iwutil::input_iterator_pack<_t...>;
		using const_iterator = iwutil::input_iterator_pack<const _t...>;
	private:
		using arrays_t = std::tuple<_t*...>;

		std::size_t m_size;
		arrays_t   m_arrays;

	public:
		array_pack() 
			: m_size(0)
			, m_arrays(arrays_t(new _t[_capacity]...)) {}

		array_pack(
			const array_pack& copy)
			: m_size(copy.m_size)
			, m_arrays(arrays_t(new _t[_capacity]...)) 
		{
			copy_streams(copy.m_arrays);
		}

		array_pack(
			array_pack&& copy)
			: m_size(copy.m_size)
			, m_arrays(arrays_t(new _t[_capacity]...)) 
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
			m_arrays = arrays_t(new _t[_capacity]...);
			copy_streams(copy.m_arrays);
		}

		array_pack& operator=(
			array_pack&& copy)
		{
			m_size = copy.m_size;
			m_arrays = arrays_t(new _t[_capacity]...);
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

		void insert(
			const _t&... components)
		{
			if (!is_full()) {
				data = insert_into_streams(components...);
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
			return m_size == _capacity;
		}

		std::size_t size() {
			return m_size;
		}

		std::size_t capacity() {
			return _capacity;
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

		struct insert_array_const {
			template<
				typename _t,
				typename _d>
				void operator()(
					_t&& stream,
					const _d& data,
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

		void insert_into_streams(
			const _t&... data) 
		{
			iwutil::foreach<
				insert_array_const,
				arrays_t,
				std::tuple<const _t&...>,
				archetype_t::size>
			(
				m_arrays,
				std::tuple<const _t&...>(data...),
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
				_capacity
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