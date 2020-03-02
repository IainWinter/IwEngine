#pragma once 

#include "iw/util/iwutil.h"
#include <cstddef>

namespace iw {
namespace util {
	class linear_allocator {
	private:
		void*  m_memory;
		size_t m_capacity;
		size_t m_minCapacity;
		size_t m_peak;
		size_t m_cursor;

	public:
		IWUTIL_API
		linear_allocator(
			size_t size);

		GEN_5(IWUTIL_API, linear_allocator);

		template<
			typename _t>
		_t* alloc(
			size_t count = 1,
			size_t alignment = 0)
		{
			return (_t*)alloc(sizeof(_t) * count, alignment);
		}

		IWUTIL_API
		void* alloc(
			size_t size,
			size_t alignment = 0);

		IWUTIL_API
		void resize(
			size_t size);

		IWUTIL_API
		void reset();

		inline char* memory() const { // make these not inline
			return (char*)m_memory;
		}

		inline size_t capacity() const {
			return m_capacity;
		}

		inline size_t peak() const {
			return m_peak;
		}

		inline size_t size() const {
			return m_cursor;
		}
	};
}

	using namespace util;
}
