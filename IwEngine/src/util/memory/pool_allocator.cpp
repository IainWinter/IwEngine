#include "iw/util/memory/pool_allocator.h"

namespace iw {
namespace util {
	pool_allocator::pool_allocator(
		size_t pageSize,
		size_t pageSizeScale
	)
		: m_root (new page(nullptr, pageSize, pageSizeScale))
	{}

	pool_allocator::~pool_allocator() {
		delete m_root;
	}

	pool_allocator::pool_allocator(
		pool_allocator&& move
	) noexcept
		: m_root (move.m_root)
	{
		move.m_root = nullptr;
	}

	pool_allocator& pool_allocator::operator=(
		pool_allocator&& move
	) noexcept
	{
		m_root = move.m_root;
		move.m_root = nullptr;
		return *this;
	}

	// Page 

	pool_allocator::page::page(
		page* previous,
		size_t capacity,
		size_t expansion
	)
		: m_memory    ((char*)malloc(capacity))
		, m_capacity  (capacity)
		, m_expansion (expansion)
		, m_size      (0)
		, m_previous  (previous)
		, m_next      (nullptr)
	{
		reset();
	}

	pool_allocator::page::~page()
	{
		delete m_next;

		::free(m_memory);

		m_memory   = nullptr;
		m_next     = nullptr;
		m_previous = nullptr;
		m_freelist.clear();
	}

	void* pool_allocator::page::alloc(
		size_t size)
	{
		if (m_freelist.size() == 0) {
			return alloc_to_next(size);
		}

		auto itr = m_freelist.begin();

		while (itr != m_freelist.end()
			&& itr->size < size)
		{
			++itr;
		}

		if (itr == m_freelist.end()) {
			return alloc_to_next(size);
		}

		char* ptr = itr->mem;

		if (itr->size > size) {
			itr->mem  += size;
			itr->size -= size;
		}

		else {
			m_freelist.erase(itr);
		}

		memset(ptr, 0, size);

		m_size += size;

		return ptr;
	}

	bool pool_allocator::page::free(
		void* addr,
		size_t size)
	{
		if (   addr >= m_memory
			&& addr <= m_memory + m_capacity - size)
		{
			freemem& new_mem = *m_freelist.emplace(m_freelist.begin(), (char*)addr, size);

			auto itr = m_freelist.begin();
			auto end = m_freelist.end();
			++itr;

			while (itr != end) {
				char* mem_min = new_mem.mem;
				char* mem_max = new_mem.mem + new_mem.size;
				char* itr_min = itr->mem;
				char* itr_max = itr->mem + itr->size;

				if (   (mem_min <= itr_max && mem_max >= itr_min)
					|| (mem_min <  itr_min && mem_max >  itr_max))
				{
					char* min = mem_min < itr_min ? mem_min : itr_min;
					char* max = mem_max > itr_max ? mem_max : itr_max;

					new_mem.mem  = min;
					new_mem.size = max - min;

					m_freelist.erase(itr);

					itr = m_freelist.begin();
					end = m_freelist.end();
				}

				++itr;
			}

#ifdef IW_DEBUG
			memset(addr, 0xff, size);
#endif

			m_size -= size;

			if (   m_previous
				&& m_freelist.size() == 1)
			{
				const freemem& free = m_freelist.front();
				if (   free.mem  == m_memory
					&& free.size == m_capacity)
				{
					m_previous->m_next = m_next;
					if (m_next) {
						m_next->m_previous = m_previous;
					}

					m_next = nullptr;
					delete this; // ? is this even valid ?
				}
			}

			return true;
		}

		else {
			if (m_next) {
				return m_next->free(addr, size);
			}
		}

		return false;
	}

	void pool_allocator::page::reset() {	
		m_freelist.clear();
		if (m_memory) {
			m_freelist.emplace_back(m_memory, m_capacity);
			m_size = 0;

#ifdef IW_DEBUG
			memset(m_memory, 0xee, m_capacity);
#endif

			//delete m_next;
			//m_next = nullptr;
		}

		if (m_next){
			m_next->reset();
		}
	}

	size_t pool_allocator::page::size() const {
		return m_size + (m_next ? m_next->size() : 0);
	}

	void* pool_allocator::page::alloc_to_next(
		size_t size)
	{
		if (m_next == nullptr) 
		{
			size_t capacity = m_capacity * m_expansion;
			while (size > capacity) capacity *= m_expansion;
			m_next = new page(this, capacity, m_expansion);
		}

		return m_next->alloc(size);
	}
}
}
