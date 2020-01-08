#pragma once 

#include "iw/util/iwutil.h"
#include <cstddef>

namespace iw {
namespace util {
	class IWUTIL_API linear_allocator {
	private:
		void*  m_memory;
		size_t m_capacity;
		size_t m_minCapacity;
		size_t m_peak;
		size_t m_cursor;
		size_t m_resets;
		size_t m_resetsToRealloc;

	public:
		linear_allocator(
			size_t size,
			size_t resetsToRealloc = 0);

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

		void resize(
			size_t size);

		void reset(
			bool clean = false);

		inline char* memory() {
			return (char*)m_memory;
		}

		inline size_t capacity() {
			return m_capacity;
		}

		inline size_t peak() {
			return m_peak;
		}

		inline size_t size() {
			return m_cursor;
		}
	};
}

	using namespace util;
}
