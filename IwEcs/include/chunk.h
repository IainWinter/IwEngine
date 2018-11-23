#pragma once

#include "iwecs.h"
#include "IwUtil/input_iterator.h"
#include "IwUtil/bimap.h"

namespace iwecs {
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