#pragma once

#include <assert.h>
#include "input_iterator.h"

namespace iwutil {
	template<
		typename _t,
		size_t _capacity>
	class compact_array {
	public:
		using iterator = input_iterator<_t>;
		using const_iterator = input_iterator<const _t>;
	private:
		_t* m_array;
		size_t m_size;
	public:
		compact_array()
			: m_array(new _t[_capacity])
			, m_size(0) {}

		compact_array(
			const compact_array& copy)
		{
			m_array = new _t[_capacity];
			m_size = copy.m_size;
			memcpy(m_array, copy.m_array, _capacity);
		}

		compact_array(
			compact_array&& copy)
			: m_array(std::move(copy.m_array))
			, m_size(copy.m_size)
		{
			copy.m_array = nullptr;
			copy.m_size = 0;
		}

		~compact_array() {
			delete[] m_array;
		}

		compact_array& operator=(
			const compact_array& copy)
		{
			if (this != &copy) {
				delete[] m_array;
				m_array = new _t[_capacity];
				m_size = copy.m_size;
				memcpy(m_array, copy.m_array, _capacity);
			}

			return *this;
		}

		compact_array& operator=(
			compact_array&& copy)
		{
			if (this != &copy) {
				delete[] m_array;
				m_array = copy.m_array;
				m_size = copy.m_size;
				copy.m_array = nullptr;
				copy.m_size = 0;
			}

			return *this;
		}

		void push_back(
			_t&& val)
		{
			assert(_capacity > m_size);
			m_array[m_size] = std::forward<_t>(val);
			m_size++;
		}

		void push_back(
			const _t& val)
		{
			assert(_capacity > m_size);
			m_array[m_size] = val;
			m_size++;
		}

		void erase(
			unsigned int index)
		{
			assert(index >= 0);
			assert(index <  m_size);

			if (index != m_size - 1) {
				m_array[index] = std::forward<_t>(m_array[m_size - 1]);
			}

			m_size--;
		}

		size_t size() const {
			return m_size;
		}

		iterator begin() {
			return iterator(m_array);
		}

		const_iterator cbegin() const {
			return const_iterator(m_array);
		}

		iterator end() {
			return iterator(m_array + m_size);
		}

		const_iterator cend() const {
			return const_iterator(m_array + m_size);
		}
	};
}