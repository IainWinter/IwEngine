#pragma once

#include "iw/util/iwutil.h"
#include "iw/util/memory/ref.h"
#include <cstddef>
#include <malloc.h>
#include <list>
#include <mutex>

#include "iw/log/logger.h"

namespace iw {
namespace util {
	class pool_allocator {
	private:
		class page {
		private:
			struct freemem {
				char* mem;
				size_t size;

				freemem(
					char* m,
					size_t s)
					: mem(m)
					, size(s)
				{}
			};

			size_t m_capacity;
			size_t m_size;

			page* m_previous;

		public:
			page* m_next;
			char* m_memory;
			std::list<freemem> m_freelist;

			page(
				page* previous,
				size_t size);

			~page();

			void* alloc(
				size_t size);

			bool free(
				void* addr,
				size_t size);

			void reset();
			size_t size() const;

		private:
			void* alloc_to_next(
				size_t size);
		};
	private:
		page*  m_root;
		size_t m_pageSizeHint;
		std::mutex m_mutex;

	public:
		IWUTIL_API pool_allocator(size_t pageSize);

		IWUTIL_API ~pool_allocator();
		IWUTIL_API pool_allocator(pool_allocator&&) noexcept;
		IWUTIL_API pool_allocator(const pool_allocator&) = delete;
		IWUTIL_API pool_allocator& operator=(pool_allocator&&) noexcept;
		IWUTIL_API pool_allocator& operator=(const pool_allocator&) = delete;

		template<
			typename _t>
		_t* alloc_c(
			size_t count = 1)
		{
			return (_t*)alloc(count * sizeof(_t));
		}

		template<
			typename _t>
		ref<_t[]> alloc_ref_c(
			size_t count = 1)
		{
			return std::static_pointer_cast<_t[], void>(alloc_ref(count * sizeof(_t)));
		}

		template<
			typename _t>
		bool free_c(
			_t* addr,
			size_t count = 1)
		{
			addr->~_t();
			return free((void*)addr, count * sizeof(_t));
		}

		template<
			typename _t>
		_t* alloc(
			size_t size = sizeof(_t))
		{
			return (_t*)alloc(size);
		}

		template<
			typename _t>
		ref<_t> alloc_ref(
			size_t size = sizeof(_t))
		{
			return std::static_pointer_cast<_t, void>(alloc_ref(size));
		}

		template<
			typename _t>
		bool free(
			_t* addr,
			size_t size = sizeof(_t))
		{
			addr->~_t();
			return free((void*)addr, size);
		}

		IWUTIL_API
		void* alloc(
			size_t size);

		IWUTIL_API
		ref<void> alloc_ref(
			size_t size);

		IWUTIL_API
		bool free(
			void* addr,
			size_t size);

		IWUTIL_API void reset();
		IWUTIL_API size_t page_size_hint() const;
		IWUTIL_API size_t acitive_size() const;

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
