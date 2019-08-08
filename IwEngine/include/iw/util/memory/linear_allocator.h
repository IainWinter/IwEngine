#pragma once 

#include "iw/util/iwutil.h"
#include <cstddef>

namespace iwutil {
	class IWUTIL_API linear_allocator {
	private:
		void*       m_memory;
		size_t m_capacity;
		size_t m_peak;
		size_t m_cursor;

	public:
		linear_allocator(
			size_t size);

		~linear_allocator();

		template<
			typename _t>
		_t* alloc(
			size_t count = 1,
			size_t alignment = 0)
		{
			return (_t*)alloc(sizeof(_t) * count, alignment);
		}

		void* alloc(
			size_t size,
			size_t alignment = 0);

		void reset();

		inline size_t peak() {
			return m_peak;
		}

		inline size_t size() {
			return m_cursor;
		}

		inline size_t capacity() {
			return m_capacity;
		}
	};
}
