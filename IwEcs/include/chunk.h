#pragma once

#include "iwecs.h"
#include "IwUtil/pack_input_iterator.h"
#include "IwUtil/archtype.h"
#include "IwUtil/tuple_iteration.h"
#include <tuple>

namespace iwecs {
	template<
		std::size_t _size>
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

	template<
		std::size_t _size_in_bytes, 
		typename... _t>
	class chunk {
	public:
		using archtype_t = iwutil::archtype<_t...>;
		using data_t     = chunk_data<archtype_t::size>;

		using iterator       = iwutil::pack_input_iterator<_t...>;
		using const_iterator = iwutil::pack_input_iterator<const _t...>;

		static constexpr std::size_t capacity 
			= _size_in_bytes / archtype_t::size_in_bytes;
	private:
		using streams_t = std::tuple<_t*...>;

		std::size_t m_size;
		streams_t   m_streams;

	public:
		chunk() 
		  : m_size(0),
			m_streams(streams_t(new _t[capacity]...)) {}

		chunk(
			const chunk& copy)
		  : m_size(copy.m_size),
			m_streams(streams_t(new _t[capacity]...)) 
		{
			copy_streams(copy.m_streams);
		}

		chunk(
			chunk&& copy)
		  : m_size(copy.m_size),
			m_streams(streams_t(new _t[capacity]...)) 
		{
			copy_streams(copy.m_streams);
		}

		~chunk() {
			delete_streams();
		}

		chunk& operator=(
			const chunk& copy)
		{
			m_size = copy.m_size;
			m_streams = streams_t(new _t[capacity]...);
			copy_streams(copy.m_streams);
		}

		chunk& operator=(
			chunk&& copy)
		{
			m_size = copy.m_size;
			m_streams = streams_t(new _t[capacity]...);
			copy_streams(copy.m_streams);
		}

		data_t insert(
			_t&&... components)
		{
			data_t data;
			if (!is_full()) {
				data = insert_into_streams(std::forward<_t>(components)...);
				m_size++;
			}

			return data;
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
			return m_size == capacity;
		}

		std::size_t size() {
			return m_size;
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
				typename _t>
			void operator()(
				_t&& stream,
				_t&& data,
				std::size_t index)
			{
				stream[index] = data;
			}
		};

		template<
			std::size_t... _tuple_index>
		data_t insert_into_streams(
			std::index_sequence<_tuple_index...>,
			_t&&... data)
		{

			iwutil::foreach<insert_array, streams_t, std::tuple<_t&&...>, archtype_t::size>(insert_array(), m_streams, std::forward_as_tuple<_t...>(data...));


			auto e = {
				(std::get<_tuple_index>(m_streams)[m_size] = data, 0)...
			};

			void* components[archtype_t::size] = {
				(void*)&std::get<_tuple_index>(m_streams)[m_size]...
			};

			return data_t(m_size, components);
		}

		data_t insert_into_streams(
			_t&&... data) 
		{
			iwutil::foreach<insert_array, streams_t, std::tuple<_t...>, archtype_t::size>(m_streams, std::forward_as_tuple(data...), m_size);
			//return insert_into_streams(
			//	std::make_index_sequence<sizeof...(_t)>{},
			//	std::forward<_t>(data)...
			//);
		}

		void remove_from_streams(
			std::size_t remove_index) 
		{
			iwutil::foreach<collapse_array, streams_t, archtype_t::size>(m_streams, remove_index, m_size - 1);
		}

		void delete_streams() {
			iwutil::foreach<functors::erase_array, streams_t, archtype_t::size>(m_streams);
		}

		template<
			std::size_t... _tuple_index>
		void copy_streams(
			std::index_sequence<_tuple_index...>,
			const streams_t& copy)
		{
			auto e = {(
				memcpy(
					std::get<_tuple_index>(m_streams), 
					std::get<_tuple_index>(copy),
					capacity),
				0)... //capacity might be wrong. May need size in bytes
			};
		}

		void copy_streams(
			const streams_t& copy)
		{
			copy_streams(std::make_index_sequence<archtype_t::size>{}, copy);
		}

		template<std::size_t... _tuple_index>
		iterator get_iterator(
			std::index_sequence<_tuple_index...>,
			index_t index)
		{
			return iterator(std::get<_tuple_index>(m_streams) + index...);
		}

		iterator get_iterator() {
			return get_iterator(std::make_index_sequence<archtype_t::size>{}, 0);
		}

		iterator get_iterator_end() {
			return get_iterator(std::make_index_sequence<archtype_t::size>{}, m_size);
		}
	};
}