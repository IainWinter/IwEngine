#pragma once

#include "iw/util/iwutil.h"
#include "iw/util/memory/ref.h"
#include <cstddef>
#include <malloc.h>
#include <list>
#include <mutex>
#include <functional>
#include "iw/log/logger.h"

namespace iw {
namespace util {
	class pool_allocator {
	private:
		struct page {
			struct freemem {
				char* mem;
				size_t size;

				freemem(char* m, size_t s)
					: mem(m), size(s)
				{}
			};

			size_t m_capacity;
			size_t m_size;
			size_t m_expansion;

			page* m_previous;

			page* m_next;
			char* m_memory;
			std::list<freemem> m_freelist;

			IWUTIL_API page(page* previous, size_t size, size_t expansion);
			IWUTIL_API ~page();

			IWUTIL_API void* alloc(size_t size);
			IWUTIL_API bool free(void* addr, size_t size);

			IWUTIL_API void reset();
			IWUTIL_API size_t size() const;

		private:
			void* alloc_to_next(size_t size);
		};
	private:
		page*  m_root;
		std::mutex m_mutex;

	public:
		IWUTIL_API pool_allocator(size_t pageSize, size_t pageSizeScale);

		IWUTIL_API ~pool_allocator();
		IWUTIL_API pool_allocator(pool_allocator&&) noexcept;
		IWUTIL_API pool_allocator(const pool_allocator&) = delete;
		IWUTIL_API pool_allocator& operator=(pool_allocator&&) noexcept;
		IWUTIL_API pool_allocator& operator=(const pool_allocator&) = delete;

		template<typename _t>
		_t* alloc_c(size_t count = 1)
		{
			return (_t*)alloc(count * sizeof(_t));
		}

		template<typename _t>
		ref<_t[]> alloc_ref_c(size_t count = 1)
		{
			return std::static_pointer_cast<_t[], void>(alloc_ref(count * sizeof(_t)));
		}

		template<typename _t>
		bool free_c(_t* addr, size_t count = 1)
		{
			addr->~_t();
			return free((void*)addr, count * sizeof(_t));
		}

		template<typename _t>
		_t* alloc(size_t size = sizeof(_t))
		{
			return (_t*)alloc(size);
		}

		template<typename _t>
		ref<_t> alloc_ref(size_t size = sizeof(_t))
		{
			return std::static_pointer_cast<_t, void>(alloc_ref(size));
		}

		template<typename _t>
		bool free(_t* addr, size_t size = sizeof(_t))
		{
			addr->~_t();
			return free((void*)addr, size);
		}

		void* alloc(size_t size)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_root->alloc(size);
		}

		ref<void> alloc_ref(size_t size)
		{
			return ref<void>(
				alloc<void>(size),
				std::bind(&pool_allocator::free<void>, this, std::placeholders::_1, size)
			);
		}

		bool free(void* addr, size_t size)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_root->free(addr, size);
		}

		void reset()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_root->reset();
		}

		size_t page_size_hint() const { return m_root->m_capacity; }
		size_t expansion_rate() const { return m_root->m_capacity; }
		size_t acitive_size()   const { return m_root->size(); }

		void log() const {
			page* page = m_root;

			char* mem = page->m_memory;
			auto list = page->m_freelist;

			int i = 1;

			LOG_INFO << "Pool allocator - " << acitive_size();
			while (page) {
				LOG_INFO << "Page " << i;
				for (auto f : list) {
					LOG_INFO << " - " << f.size << " " << (void*)f.mem;
				}

				page = page->m_next;
				i++;
			}
		}
	};
}

	using namespace util;
}
