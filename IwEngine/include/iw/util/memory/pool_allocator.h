#pragma once

#include "iw/util/iwutil.h"
#include <cstddef>
#include <malloc.h>
#include <list>

namespace iwutil {
	class IWUTIL_API pool_allocator {
	private:
		struct item {
			void* Ptr;
			item* Next;
		};

		class page {
		private:
			void* m_memory;
			item* m_freelist;
			item* m_free;
			page* m_next;

		public:
			page(
				size_t size,
				size_t count,
				size_t run);

			~page();

			void* alloc(
				size_t size,
				size_t run);

			page(const page&)            = delete;
			page& operator=(const page&) = delete;
		};

		page* m_root;
		size_t m_pageSize;
		size_t m_itemSize;

	public:
		pool_allocator(
			size_t pageSize,
			size_t itemSize);

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
		_t* alloc() {
			return (_t*)alloc();
		}

		void* alloc();

		inline size_t page_size() const {
			return m_pageSize;
		}

		inline size_t item_size() const {
			return m_itemSize;
		}

		inline size_t page_capacity() const {
			return m_pageSize / m_itemSize;
		}
	};
}
