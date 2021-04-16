#include "iw/util/memory/pool_allocator.h"
#include "iw/log/logger.h"
#include <functional>

using namespace std::placeholders;

namespace iw {
namespace util {
	pool_allocator::pool_allocator(
		size_t pageSize)
		: m_root(new page(nullptr, pageSize))
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
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_root->alloc(size);
	}

	ref<void> pool_allocator::alloc_ref(
		size_t size)
	{
		return ref<void>(alloc<void>(size), std::bind(&pool_allocator::free<void>, this, _1, size));
	}

	bool pool_allocator::free(
		void* addr,
		size_t size)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_root->free(addr, size);
	}

	void pool_allocator::reset() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_root->reset();
	}

	size_t pool_allocator::page_size() const {
		return m_pageSize;
	}

	size_t pool_allocator::acitive_size() const {
		return m_root->size();
	}

	// Page 

	pool_allocator::page::page(
		page* previous,
		size_t size)
		: m_memory((char*)malloc(size))
		, m_capacity(size)
		, m_size(0)
		, m_previous(previous)
		, m_next(nullptr)
	{
		reset();

		//LOG_INFO << "Appended " << m_capacity << " byte page to   pool allocator";
	}

	pool_allocator::page::~page() {
		delete m_next;

		::free(m_memory);

		m_memory   = nullptr;
		m_next     = nullptr;
		m_previous = nullptr;
		m_freelist.clear();

		//LOG_INFO << "Removed  " << m_capacity << " byte page from pool allocator";
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

	char* pool_allocator::page::memory() const {
		return m_memory;
	}

	size_t pool_allocator::page::size() const {
		return m_size + (m_next ? m_next->size() : 0);
	}

	const std::list<pool_allocator::page::freemem>& pool_allocator::page::freelist() const {
		return m_freelist;
	}

	pool_allocator::page* pool_allocator::page::next() {
		return m_next;
	}

	void* pool_allocator::page::alloc_to_next(
		size_t size)
	{
		if (m_next == nullptr) {
			m_next = new page(this, m_capacity);
		}

		return m_next->alloc(size);
	}
}
}
