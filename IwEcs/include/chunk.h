#pragma once

#include "iwecs.h"
#include <tuple>
#include <memory>
#include "IwUtil/input_iterator.h"

namespace iwecs {
	//Switch to using a memory manager system. This is pritty much a linear allocator
	template<typename _t>
	class chunk {
	private:
		std::size_t m_size;
		std::size_t m_capacity;
		std::shared_ptr<_t> m_start;
		_t* m_end;
		_t* m_pointer;
	public:
		using iterator = iwutil::input_iterator<_t>;

		chunk(std::size_t prefered_size)
			: m_size(prefered_size - prefered_size % sizeof(_t)),
			m_capacity(m_size / sizeof(_t)),
			m_start((_t*)malloc(m_size)),
			m_end(m_start.get() + m_capacity),
			m_pointer(m_start.get()) {
		}

		chunk(const chunk& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_start(copy.m_start),
			m_end(copy.m_end),
			m_pointer(copy.m_pointer) {
		}

		chunk(chunk&& copy)
			: m_size(copy.m_size),
			m_capacity(copy.m_capacity),
			m_start(copy.m_start),
			m_end(copy.m_end),
			m_pointer(copy.m_pointer) {
		}

		chunk& operator==(const chunk& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			m_start = copy.m_start;
			m_end = copy.m_end;
			m_pointer = copy.m_pointer;
		}

		chunk& operator==(chunk&& copy) {
			m_size = copy.m_size;
			m_capacity = copy.m_capacity;
			m_start = copy.m_start;
			m_end = copy.m_end;
			m_pointer = copy.m_pointer;
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