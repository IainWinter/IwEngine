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
	class IWUTIL_API pool_allocator {
	public:
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

			char* m_memory;
			std::list<freemem> m_freelist;

			size_t m_capacity;
			size_t m_size;

			page* m_previous;
			page* m_next;

		public:
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

			char* memory() const;
			size_t size() const;
			const std::list<freemem>& freelist() const;
			page* next();

		private:
			void* alloc_to_next(
				size_t size);

			//page(
			//	const page&) = delete;

			//page& operator=(
			//	const page&) = delete;
		};
	private:
		page*  m_root;
		size_t m_pageSize;
		std::mutex m_mutex;

	public:
		pool_allocator(
			size_t pageSize);

		pool_allocator(
			pool_allocator&&) noexcept;

		pool_allocator(
			const pool_allocator&) = delete;

		~pool_allocator();

		pool_allocator& operator=(
			pool_allocator&&) noexcept;

		pool_allocator& operator=(
			const pool_allocator&) = delete;

		template<
			typename _t>
		_t* alloc(
			size_t size = sizeof(_t))
		{
			return (_t*)alloc<void>(size);
		}

		template<
			typename _t>
		ref<_t> alloc_ref(
			size_t size = sizeof(_t))
		{
			return std::static_pointer_cast<_t, void>(alloc_ref<void>(size));
		}

		template<
			typename _t>
		bool free(
			_t* addr,
			size_t size = sizeof(_t))
		{
			addr->~_t();
			return free<void>(addr, size);
		}

		template<>
		void* alloc<void>(
			size_t size);

		template<>
		ref<void> alloc_ref<void>(
			size_t size);

		template<>
		bool free<void>(
			void* addr,
			size_t size);

		void reset();

		size_t page_size() const;
		size_t acitive_size() const;

		void log() const {
			page* page = m_root;

			char* mem = page->memory();
			auto list = page->freelist();

			int i = 1;

			LOG_INFO << "Pool allocator - " << acitive_size();
			while (page) {
				LOG_INFO << "Page " << i;
				for (auto f : list) {
					LOG_INFO << " - " << f.size << " " << (void*)f.mem;
				}

				page = page->next();
				i++;
			}
			
		}

		//size_t item_size() const {
		//	return m_itemSize;
		//}

		//size_t page_capacity() const {
		//	return m_pageSize / m_itemSize;
		//}
	};
}

	using namespace util;
}
