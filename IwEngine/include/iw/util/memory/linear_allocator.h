#pragma once 

#include "iw/util/iwutil.h"
#include <cstddef>

namespace iwutil {
	class IWUTIL_API linear_allocator {
	private:
		void*       m_memory;
		std::size_t m_capacity;
		std::size_t m_peak;
		std::size_t m_cursor;

	public:
		linear_allocator(
			std::size_t size);

		~linear_allocator();

		template<
			typename _t>
		_t* alloc(
			std::size_t count = 1,
			std::size_t alignment = 0)
		{
			return (_t*)alloc(sizeof(_t) * count, alignment);
		}

		void* alloc(
			std::size_t size,
			std::size_t alignment = 0);

		void reset();

		inline std::size_t peak() {
			return m_peak;
		}

		inline std::size_t size() {
			return m_cursor;
		}

		inline std::size_t capacity() {
			return m_capacity;
		}
	};
}
