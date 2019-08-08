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
				std::size_t size,
				std::size_t count,
				std::size_t run);

			~page();

			void* alloc(
				std::size_t size,
				std::size_t run);

			page(const page&)            = delete;
			page& operator=(const page&) = delete;
		};

		page* m_root;
		std::size_t m_pageSize;
		std::size_t m_itemSize;

	public:
		pool_allocator(
			std::size_t pageSize,
			std::size_t itemSize);

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

		inline std::size_t page_size() {
			return m_pageSize;
		}

		inline std::size_t item_size() {
			return m_itemSize;
		}

		inline std::size_t page_capacity() {
			return m_pageSize / m_itemSize;
		}
	};
}
