#include "iw/util/memory/pool_allocator.h"
#include <functional>

#include "iw/log/logger.h"

using namespace std::placeholders;

namespace iw {
	pool_allocator::pool_allocator(
		size_t pageSize)
		: m_root(new page(pageSize))
		, m_pageSize(pageSize)
	{}

	pool_allocator::pool_allocator(
		pool_allocator&& copy) noexcept
		: m_root(copy.m_root)
		, m_pageSize(copy.m_pageSize)
	{
		copy.m_root = nullptr;
		copy.m_pageSize = 0;
	}

	pool_allocator::~pool_allocator() {
		delete m_root;
	}

	pool_allocator& pool_allocator::operator=(
		pool_allocator&& copy) noexcept
	{
		m_root = copy.m_root;
		m_pageSize = copy.m_pageSize;

		copy.m_root = nullptr;
		copy.m_pageSize = 0;

		return *this;
	}

	void* pool_allocator::alloc(
		size_t size)
	{
		return m_root->alloc(size);
	}

	ref<void> pool_allocator::alloc_ref(
		size_t size)
	{
		return ref<void>(alloc(size), std::bind(&pool_allocator::free, this, _1, size));
	}

	bool pool_allocator::free(
		void* addr,
		size_t size)
	{
		return m_root->free(addr, size);
	}

	// Page 

	pool_allocator::page::page(
		size_t size)
		: m_memory((char*)malloc(size))
		, m_size(size)
		, m_next(nullptr)
	{
		if (m_memory) {
			m_freelist.emplace_back(m_memory, size);

#ifdef IW_DEBUG
			memset(m_memory, 0xcd, size);
#endif
		}
	}

	pool_allocator::page::~page() {
		delete[] m_memory;
		delete m_next;
	}

	void* pool_allocator::page::alloc(
		size_t size)
	{
		if (   m_freelist.empty()
			|| m_freelist.front().size < size)
		{
			if (m_next == nullptr) {
				m_next = new page(m_size);
			}

			return m_next->alloc(size);
		}

		auto itr = m_freelist.begin();
		while (size > itr->size) {
			++itr;
		}

		char* ptr  = itr->mem;

		if (itr->size > size) {
			itr->mem  += size;
			itr->size -= size;
		}

		else {
			m_freelist.pop_front();
		}

		memset(ptr, 0, size);

		return ptr;
	}

	bool pool_allocator::page::free(
		void* addr,
		size_t size)
	{
		if (   addr >= m_memory
			&& addr <= m_memory + m_size - size)
		{
			auto itr = m_freelist.begin();
			while (itr != m_freelist.end()
				&& itr->mem != (char*)addr + size)
			{
				++itr;
			}

			if (itr != m_freelist.end()) {
				itr->mem  -= size;
				itr->size += size;
			}
			
			else {
				m_freelist.emplace_front((char*)addr, size);
			}

#ifdef IW_DEBUG
			memset(addr, 0xff, size);
#endif
		}

		else {
			if (m_next) {
				return m_next->free(addr, size);
			}
		}

		return false;
	}
}
