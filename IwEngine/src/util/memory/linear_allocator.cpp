#include "iw/util/memory/linear_allocator.h"
#include "iw/log/logger.h"
#include "memory_util.h"
#include <stdlib.h>
#include <assert.h>

namespace iw {
	linear_allocator::linear_allocator(
		size_t size)
		: m_memory(malloc(size))
		, m_capacity(size)
		, m_minCapacity(size)
		, m_peak(0)
		, m_cursor(0)
	{
		assert(m_memory);
		memset(m_memory, 0, m_capacity);
	}

	linear_allocator::linear_allocator(
		const linear_allocator& copy)
		: m_memory(malloc(copy.m_capacity))
		, m_capacity(copy.m_capacity)
		, m_minCapacity(copy.m_minCapacity)
		, m_peak(copy.m_peak)
		, m_cursor(copy.m_cursor)
	{
		assert(m_memory);
		memcpy(m_memory, copy.m_memory, m_capacity);
	}

	linear_allocator::linear_allocator(
		linear_allocator&& copy) noexcept
		: m_memory(copy.m_memory)
		, m_capacity(copy.m_capacity)
		, m_minCapacity(copy.m_minCapacity)
		, m_peak(copy.m_peak)
		, m_cursor(copy.m_cursor)
	{
		copy.m_memory = nullptr;
		copy.m_capacity = 0;
		copy.m_minCapacity = 0;
		copy.m_peak = 0;
		copy.m_cursor = 0;
	}

	linear_allocator::~linear_allocator() {
		free(m_memory);
	}

	linear_allocator& linear_allocator::operator=(
		const linear_allocator& copy)
	{
		m_memory = copy.m_memory;
		m_capacity = copy.m_capacity;
		m_minCapacity = copy.m_minCapacity;
		m_peak = copy.m_peak;
		m_cursor = copy.m_cursor;

		m_memory = malloc(m_capacity);
		assert(m_memory);
		memcpy(m_memory, copy.m_memory, m_capacity);

		return *this;
	}

	linear_allocator& linear_allocator::operator=(
		linear_allocator&& copy) noexcept
	{
		m_memory          = copy.m_memory;
		m_capacity        = copy.m_capacity;
		m_minCapacity     = copy.m_minCapacity;
		m_peak            = copy.m_peak;
		m_cursor          = copy.m_cursor;

		copy.m_memory          = nullptr;
		copy.m_capacity        = 0;
		copy.m_minCapacity     = 0;
		copy.m_peak            = 0;
		copy.m_cursor          = 0;

		return *this;
	}

	void* linear_allocator::alloc(
		size_t size, 
		size_t alignment)
	{
		size_t padding  = 0;
		size_t paddress = 0;
		size_t caddress = (size_t)m_memory + m_cursor;
		if (alignment != 0 && m_cursor % alignment != 0) {
			padding = calc_padding(caddress, alignment);
		}

		size_t padsize = padding + size;
		if (m_cursor + padsize > m_capacity) {
			return nullptr;
		}

		m_cursor += padsize;

		//LOG_DEBUG << "Allocating " << size << " bytes at " << (void*)caddress << " with " << padding << " bytes padding"

		if (m_peak < m_cursor) {
			m_peak = m_cursor;
		}

		//memset((void*)caddress, 0, padsize); //reset memory of alloc not nessesary but nice. TODO: lookout for preformance hit

		return (void*)(caddress + padding);
	}

	void linear_allocator::resize(
		size_t size)
	{
		void* memory = realloc(m_memory, size);
		if (!memory) {
			memory = malloc(size);
			assert(memory);

			memcpy(memory, m_memory, size < m_cursor ? size : m_cursor);
			free(m_memory);
		}

		m_memory = memory;
		m_capacity = size;

#ifdef IW_DEBUG
		LOG_INFO << "Resized linear allocator to " << m_capacity;
#endif
	}

	void linear_allocator::reset() {
		memset(m_memory, 0, m_cursor);
		m_cursor = 0;
	}
}
