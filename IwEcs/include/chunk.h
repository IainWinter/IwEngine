#pragma once

#include "iwecs.h"
#include <tuple>

namespace iwecs {
	//Switch to using a memory manager system. This is pritty much a linear allocator
	template<typename _t>
	class chunk {
	private:
		std::size_t m_size;
		std::size_t m_capacity;
		_t* m_start;
		_t* m_end;
		_t* m_pointer;
	public:
		chunk(std::size_t prefered_size)
		  : m_size    (prefered_size - prefered_size % sizeof(_t)),
			m_capacity(m_size / sizeof(_t)),
			m_start   ((_t*)malloc(m_size)),
			m_end     (m_start + m_capacity),
			m_pointer (m_start) {}

		~chunk() {
			delete[] m_start;
		}

		bool is_full() {
			return m_pointer == m_end;
		}
	};
}