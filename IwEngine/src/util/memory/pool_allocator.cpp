#include "iw/util/memory/pool_allocator.h"

namespace iwutil {
	pool_allocator::pool_allocator(
		size_t pageSize, 
		size_t itemSize)
		: m_pageSize(pageSize)
		, m_itemSize(itemSize)
	{
		size_t r = pageSize % itemSize;
		pageSize -= r;

		m_root = new page(pageSize, pageSize / itemSize, itemSize);
	}

	pool_allocator::pool_allocator(
		pool_allocator&& copy) noexcept
		: m_root(copy.m_root)
		, m_pageSize(copy.m_pageSize)
		, m_itemSize(copy.m_itemSize)
	{
		copy.m_root = nullptr;
	}

	pool_allocator::~pool_allocator() {
		delete m_root;
	}

	pool_allocator& pool_allocator::operator=(
		pool_allocator&& copy) noexcept
	{
		m_root = copy.m_root;
		m_pageSize = copy.m_pageSize;
		m_itemSize = copy.m_itemSize;

		copy.m_root = nullptr;

		return *this;
	}

	bool pool_allocator::free(
		void* addr)
	{
		return m_root->free(addr, m_pageSize, m_itemSize);
	}

	void* pool_allocator::alloc() {
		return m_root->alloc(m_pageSize, m_itemSize);
	}

	// Page 

	pool_allocator::page::page(
		size_t size,
		size_t count,
		size_t run)
		: m_memory((char*)malloc(size))
		, m_next(nullptr)
	{
		if (m_memory) {
			m_freelist.reserve(size);
			for (size_t i = 0; i < size; i++) {
				m_freelist.push_back(&m_memory[i]);
			}
		}
	}

	pool_allocator::page::~page() {
		delete[] m_memory;
		delete m_next;
	}

	void* pool_allocator::page::alloc(
		size_t size,
		size_t run)
	{
		if (m_freelist.empty()) {
			if (m_next == nullptr) {
				m_next = new page(size, size / run, run);
			}

			return m_next->alloc(size, run);
		}

		char* ptr = m_freelist.back();
		m_freelist.pop_back();

#ifdef IW_DEBUG
		memset(ptr, 0, run); 
#endif
		return ptr;
	}

	bool pool_allocator::page::free(
		void* addr,
		size_t size,
		size_t run)
	{
		if (addr >= m_memory && addr <= m_memory + size - run) {
			m_freelist.push_back((char*)addr);

#ifdef IW_DEBUG
			memset(addr, 0, run);
#endif
		}

		else {
			if (m_next) {
				return m_next->free(addr, size, run);
			}
		}

		return false;
	}
}
