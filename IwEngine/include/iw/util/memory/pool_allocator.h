#pragma once

#include "iw/util/iwutil.h"
#include "iw/util/memory/smart_pointers.h"
#include <cstddef>
#include <malloc.h>
#include <list>

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
			size_t m_size;
			page* m_next;
			std::list<freemem> m_freelist;

		public:
			page(
				size_t size);

			~page();

			void* alloc(
				size_t size);

			bool free(
				void* addr,
				size_t size);

			char* memory() const {
				return m_memory;
			}

			//page(
			//	const page&) = delete;

			//page& operator=(
			//	const page&) = delete;
		};
	private:
		page*  m_root;
		size_t m_pageSize;

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

		void* alloc(
			size_t size);

		ref<void> alloc_ref(
			size_t size);

		bool free(
			void* addr,
			size_t size);

		template<
			typename _t>
		_t* alloc_t(
			size_t size = sizeof(_t))
		{
			return (_t*)alloc(size);
		}

		template<
			typename _t>
		ref<_t> alloc_ref_t(
			size_t size = sizeof(_t))
		{
			return std::static_pointer_cast<_t, void>(alloc_ref(size));
		}

		template<
			typename _t>
		bool free_t(
			_t* addr,
			size_t size = sizeof(_t))
		{
			return free(addr, size);
		}

		size_t page_size() const {
			return m_pageSize;
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
