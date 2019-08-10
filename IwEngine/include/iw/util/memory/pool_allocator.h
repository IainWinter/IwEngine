#pragma once

#include "iw/util/iwutil.h"
#include <cstddef>
#include <malloc.h>
#include <forward_list>

namespace iwutil {
	class IWUTIL_API pool_allocator {
	public:
		class page {
		private:
			char* m_memory;
			page* m_next;
			std::forward_list<char*> m_freelist;

		public:
			page(
				size_t size,
				size_t count,
				size_t run);

			~page();

			void* alloc(
				size_t size,
				size_t run);

			bool free(
				void* addr,
				size_t size,
				size_t run);

			inline char* memory() const {
				return m_memory;
			}

			page(const page&) = delete;
			page& operator=(const page&) = delete;
		};

		class iterator {
		public:
			using page_type   = page;
			using value_type  = char*;
			using pointer     = value_type*;
			using reference   = value_type&;
			using difference_type   = std::ptrdiff_t;
			using iterator_category = std::random_access_iterator_tag;
		private:
			iterator(
				page_type* page,
				value_type addr,
				value_type end)
				: m_page(page)
				, m_addr(addr)
				, m_end (end)
			{}

			friend class pool_allocator;

			page_type* m_page;
			value_type m_addr;
			value_type m_end;

		public:
			iterator(
				const iterator& copy) = default;

			iterator(
				iterator&& copy) = default;

			~iterator() = default;

			iterator& operator=(
				const iterator& copy) = default;

			iterator& operator=(
				iterator&& copy) = default;

			iterator& operator++() {
				++m_addr;
				return *this;
			}

			iterator& operator--() {
				--m_addr;
				return *this;
			}

			iterator operator++(
				int)
			{
				iterator itr(*this);
				++*this;
				return itr;
			}

			iterator operator--(
				int)
			{
				iterator itr(*this);
				--*this;
				return itr;
			}

			iterator& operator+=(
				const difference_type& value)
			{
				m_addr += value;
				return *this;
			}

			iterator& operator-=(
				const difference_type& value)
			{
				m_addr -= value;
				return *this;
			}

			iterator operator+(
				const difference_type& dif)
			{
				return iterator(m_page, m_addr + dif, m_end);
			}

			iterator operator-(
				const difference_type& dif)
			{
				return iterator(m_page, m_addr - dif, m_end);
			}

			//difference_type operator+(
			//	const iterator& itr)
			//{
			//	return (m_addr + itr.m_addr);
			//}

			//difference_type operator-(
			//	const iterator& itr)
			//{
			//	return m_addr - itr.m_addr;
			//}

			bool operator==(
				const iterator& itr) const
			{
				return m_addr == itr.m_addr;
			}

			bool operator!=(
				const iterator& itr) const
			{
				return !(m_addr == itr.m_addr);
			}

			bool operator>(
				const iterator& itr) const
			{
				return m_addr > itr.m_addr;
			}

			bool operator<(
				const iterator& itr) const
			{
				return m_addr < itr.m_addr;
			}

			bool operator>=(
				const iterator& itr) const
			{
				return !(m_addr < itr.m_addr);
			}

			bool operator<=(
				const iterator& itr) const
			{
				return !(m_addr > itr.m_addr);
			}

			pointer operator->() {
				return &m_addr;
			}

			reference operator*() {
				return *operator->();
			}

			//reference operator[](
			//	size_t index)
			//{
			//	return (char*)m_addr + index;
			//}
		};
	private:
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

		bool free(
			void* addr);

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
