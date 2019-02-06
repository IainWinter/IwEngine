#pragma once

#include "tight_container.h"
#include "input_iterator.h"

namespace iwutil {
	template<
		typename _t,
		std::size_t _capacity>
	class tight_array : public tight_container<_t> {
	private:
		_t* m_array;

	public:
		tight_array()
			: m_array(new _t[_capacity]) {}
		
		tight_array(
			tight_array&& copy)
			: m_array(std::move(copy.m_array))
			, m_size(copy.m_size)
		{
			copy.m_array = nullptr;
			copy.m_size = 0;
		}

		tight_array(
			const tight_array& copy)
		{
			m_array = new _t[_capacity];
			m_size  = copy.m_size;
			std::copy(m_array, copy.m_array, _capacity * sizeof(_t));
		}

		~tight_array() {
			delete[] m_array;
		}

		tight_array& operator=(
			tight_array&& copy)
		{
			if (this != &copy) {
				delete[] m_array;
				m_array = copy.m_array;
				m_size  = copy.m_size;
				copy.m_array = nullptr;
				copy.m_size = 0;
			}

			return *this;
		}

		tight_array& operator=(
			const tight_array& copy) 
		{
			if (this != &copy) {
				delete[] m_array;
				m_array = new _t[_capacity];
				m_size  = copy.m_size;
				std::copy(m_array, copy.m_array, _capacity * sizeof(_t));
			}

			return *this;
		}

		//Insert data at m_size in the array
		std::size_t push_back(
			_t&& val)
		{
			m_array[m_size] = std::forward<_t>(val);
			return m_size++;
		}

		//Insert data at m_size in the array
		std::size_t push_back(
			const _t& val)
		{
			m_array[m_size] = val;
			return m_size++;
		}

		//Replaces nth element with element at m_size - 1
		//	and decrements the size
		void erase(
			std::size_t index)
		{
			if (m_size > 0) {
				if (index != m_size - 1) {
					m_array[index] = std::forward<_t>(m_array[m_size - 1]);
				}

				m_size--;
			}
		}

		_t* data() {
			return m_array;
		}
	};
}