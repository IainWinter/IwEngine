#pragma once

#include "iwecs.h"
#include "IwUtil/pack_input_iterator.h"
#include "IwUtil/archtype.h"
#include "IwUtil/tuple_iteration.h"
#include <tuple>
#include <array>

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
			void* (&data_)[_size])
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
				typename _t,
				typename _d>
			void* operator()(
				_t&& stream,
				_d&& data,
				std::size_t index)
			{
				stream[index] = data;
				return (void*)stream;
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

		struct get {
			template<
				typename _t>
			_t operator()(
				_t&& stream,
				std::size_t index)
			{
				return stream + index;
			}
		};

		data_t insert_into_streams(
			_t&&... data) 
		{
			std::array<void*, archtype_t::size> components = iwutil::geteach<
				insert_array,
				streams_t,
				std::array<void*, archtype_t::size>,
				std::tuple<_t&&...>,
				archtype_t::size>
			(
				m_streams,
				std::forward_as_tuple<_t...>(std::forward<_t>(data)...),
				m_size
			);

			return data_t(m_size, components._Elems);
		}

		void remove_from_streams(
			std::size_t remove_index) 
		{
			iwutil::foreach<
				collapse_array,
				streams_t,
				archtype_t::size>
			(
				m_streams,
				remove_index,
				m_size - 1
			);
		}

		void delete_streams() {
			iwutil::foreach<
				functors::erase_array,
				streams_t,
				archtype_t::size>
			(
				m_streams
			);
		}

		void copy_streams(
			const streams_t& copy)
		{
			iwutil::foreach<
				copy_array,
				streams_t,
				streams_t,
				archtype_t::size>
			(
				m_streams,
				copy,
				capacity
			);
		}

		iterator get_iterator() {
			return iwutil::geteach<
				get,
				streams_t,
				iterator,
				archtype_t::size>
			(
				m_streams,
				0
			);
		}

		iterator get_iterator_end() {
			return iwutil::geteach<
				get,
				streams_t,
				iterator,
				archtype_t::size>
			(
				m_streams,
				m_size
			);
		}
	};
}