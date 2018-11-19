#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"

namespace iwecs {
	//Find better name for this
	template<typename _t>
	class chunk {
	private:
		std::size_t m_size;
		std::size_t m_capacity;
		std::unordered_map<std::size_t, _t*> m_index;
		_t* m_start;
		_t* m_end;
		_t* m_pointer;

		void init_start() {
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
			init_start();
		}

		chunk(const chunk& copy)
		  : m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_index(copy.m_index)
		{
			init_start();
			memcpy(m_start, copy.m_start, m_size);
		}

		chunk(chunk&& copy)
		  : m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_index(copy.m_index)
		{
			init_start();
			memcpy(m_start, copy.m_start, m_size);
		}

		chunk& operator=(const chunk& copy) {
			m_size     = copy.m_size;
			m_capacity = copy.m_capacity;
			m_index    = copy.m_index;
			init_start();
			memcpy(m_start, copy.m_start, m_size);
		}

		chunk& operator=(chunk&& copy) {
			m_size     = copy.m_size;
			m_capacity = copy.m_capacity;
			m_index    = copy.m_index;
			init_start();
			memcpy(m_start, copy.m_start, m_size);
		}

		~chunk() {
			delete[] m_start;
		}

		void insert(std::size_t key, _t&& data) {
			if (m_pointer != m_end) {
				*m_pointer = data;
				m_index.emplace(key, m_pointer);
				m_pointer++;
			}
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

namespace iwecs_o {
	template<typename _t>
	class chunk {
	private:
		std::size_t m_size;
		std::size_t m_capacity;
		_t* m_start;
		_t* m_end;
		_t* m_pointer;

		void init() {}

		void copy_init() {}
	public:
		using iterator = iwutil::input_iterator<_t>;

		chunk(std::size_t prefered_size)
			: m_size(prefered_size - prefered_size % sizeof(_t)),
			m_capacity(m_size / sizeof(_t))
		{
			init();
		}

		chunk(const chunk& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity)
		{
			copy_init();
		}

		chunk(chunk&& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity)
		{
			copy_init();
		}

		chunk& operator==(const chunk& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			copy_init();
		}

		chunk& operator==(chunk&& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			copy_init();
		}

		bool is_full() {
			return m_pointer == m_end;
		}

		void push_back(_t&& data) {
			if (m_pointer != m_end) {
				*m_pointer = data;
				m_pointer++;
			}
		}

		void remove(std::size_t index) {
			if (m_start + index >= m_end) return; //Index out of bounds
			m_pointer = m_start + index;
		}

		iterator begin() {
			return iterator(m_start.get());
		}

		iterator end() {
			return iterator(m_end);
		}
	};
}