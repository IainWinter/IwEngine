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

	void* pool_allocator::alloc() {
		return m_root->alloc(m_pageSize, m_itemSize);
	}

	// Page 

	pool_allocator::page::page(
		size_t size,
		size_t count,
		size_t run)
		: m_memory(malloc(size))
		, m_freelist(new item[count])
		, m_next(nullptr)
		, m_free(m_freelist)
	{
		if (m_memory) {
			size_t addr = (size_t)m_memory;
			for (size_t i = 1; i < count; i++) {
				m_freelist[i - 1].Ptr = (void*)(addr);
				m_freelist[i - 1].Next = &m_freelist[i];
				addr += run;
			}

			m_freelist[count - 1].Ptr = (void*)(addr);
			m_freelist[count - 1].Next = nullptr;
		}
	}

	pool_allocator::page::~page() {
		delete[] m_memory;
		delete[] m_freelist;
		delete m_next;
	}

	void* pool_allocator::page::alloc(
		size_t size,
		size_t run)
	{
		if (m_free == nullptr) {
			if (m_next == nullptr) {
				m_next = new page(size, size / run, run);
			}

			return m_next->alloc(size, run);
		}

		item* free = m_free;
		m_free = m_free->Next;

		memset(free->Ptr, 0, run); //reset memory of alloc not nessesary but nice. TODO: lookout for preformance hit

		return free->Ptr;
	}
}
