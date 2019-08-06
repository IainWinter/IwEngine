#include "iw/util/memory/linear_allocator.h"
#include "iw/log/logger.h"
#include "memory_util.h"
#include <stdlib.h>

namespace iwutil {
	linear_allocator::linear_allocator(
		std::size_t size)
		: m_memory(malloc(size))
		, m_capacity(size)
		, m_peak(0)
		, m_cursor(0)
	{
		if (m_memory != nullptr) {
			memset(m_memory, 0, m_capacity);
		}
	}

	linear_allocator::~linear_allocator() {
		free(m_memory);
	}

	void* linear_allocator::alloc(
		std::size_t size, 
		std::size_t alignment)
	{
		std::size_t padding  = 0;
		std::size_t paddress = 0;
		std::size_t caddress = (std::size_t)m_memory + m_cursor;
		if (alignment != 0 && m_cursor % alignment != 0) {
			padding = calc_padding(caddress, alignment);
		}

		if (m_cursor + padding + size > m_capacity) {
			return nullptr;
		}

		m_cursor += padding;
		m_cursor += size;

		//LOG_DEBUG << "Allocating " << size << " bytes at " << (void*)caddress << " with " << padding << " bytes padding"

		if (m_peak < m_cursor) {
			m_peak = m_cursor;
		}

		memset((void*)caddress, 0, size + padding); //reset memory of alloc not nessesary but nice. TODO: lookout for preformance hit

		return (void*)(caddress + padding);
	}

	void linear_allocator::reset() {
		m_cursor = 0;
	}
}
