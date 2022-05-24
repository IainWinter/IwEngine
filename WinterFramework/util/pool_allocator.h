#pragma once

#include "linear_allocator.h"

struct pool_allocator : allocator
{
	std::vector<std::pair<size_t, linear_allocator*>> m_pages; // block index start, page
	size_t m_page_size_hint;
	size_t m_next_page_size;
	size_t m_next_page_mult;

	pool_allocator()
		: m_page_size_hint (0)
		, m_next_page_size (0)
		, m_next_page_mult (2)
	{}

	pool_allocator(
		size_t page_expansion
	)
		: m_page_size_hint (0)
		, m_next_page_size (0)
		, m_next_page_mult (page_expansion)
	{}

	pool_allocator(
		size_t page_size,
		size_t page_expansion
	)
		: m_page_size_hint (page_size)
		, m_next_page_size (page_size)
		, m_next_page_mult (page_expansion)
	{}

	~pool_allocator()
	{
		for (auto [_, page] : m_pages)
		{
			delete page;
		}
	}

	char* alloc_bytes(size_t size) override
	{
		linear_allocator* page = nullptr;
		for (auto [_, p] : m_pages)
		{
			if (p->has_space(size)) 
			{
				page = p;
				break;
			}
		}

		if (!page)
		{
			m_next_page_size = size > m_next_page_size ? size : m_next_page_size;

			page = new linear_allocator(m_next_page_size);
			page->m_block_size = m_block_size;

			int start_block_index = m_pages.size() == 0 ? 0 : m_pages.back().first + m_pages.back().second->block_capacity();
			m_pages.emplace_back(start_block_index, page);

			assert(m_next_page_mult > 0 && "Next page expansion is invalid");
			m_next_page_size *= m_next_page_mult;
		}

		return page->alloc_bytes(size);
	}

	void free_bytes(void* address, size_t size) override
	{
		auto itr = m_pages.begin(); // to not include algorithm for std::find
		linear_allocator* page = nullptr;
		for (; itr != m_pages.end(); ++itr)
		{
			if ((*itr).second->contains(address)) 
			{
				page = (*itr).second;
				break;
			}
		}

		assert(page && "Address not in pool");

		page->free_bytes(address, size);
		
		// if (page->is_empty()) 
		// {
		// 	delete page;
		// 	assert(m_next_page_mult > 0 && "Next page expansion is invalid");
		// 	m_next_page_size /= m_next_page_mult;
		// 	m_pages.erase(itr);                           // see above
		// }
	}

	bool has_space(size_t size) const override
	{
		for (auto [_, page] : m_pages) if (page->has_space(size)) return true;
		return false;
	}

	bool has_allocated(void* address) const override
	{
		for (auto [_, page] : m_pages) if (page->has_allocated(address)) return true;
		return false;
	}

	bool contains(void* address) const override
	{
		for (auto [_, page] : m_pages) if (page->contains(address)) return true;
		return false;
	}

	bool contains_block(size_t index) const override
	{
		for (auto [_, page] : m_pages) if (page->contains_block(index)) return true;
		return false;
	}

	size_t get_block_index(void* address) const override
	{
		for (auto [page_index_start, page] : m_pages)
		{
			if (page->contains(address))
			{
				return page_index_start + page->get_block_index(address);
			}
		}

		assert(false && "Address is not in allocator");
		throw nullptr;
	}

	void* get_block_address(size_t index) const override
	{
		for (auto [page_index_start, page] : m_pages)
		{
			if (page->contains_block(index - page_index_start)) // valid if negitive or overflow
			{
				return page->get_block_address(index - page_index_start);
			}
		}

		assert(false && "Address is not in allocator");
		throw nullptr;
	}

	void reset() override
	{
		for (auto [_, page] : m_pages)
		{
			delete page;
		}

		m_pages.clear();
		m_next_page_size = m_page_size_hint;
	}

	size_t capacity() const override
	{
		size_t size = 0;
		for (auto [_, page] : m_pages) size += page->capacity();
		return size;
	}

	bool is_empty() const override
	{
		for (auto [_, page] : m_pages) if (!page->is_empty()) return false;
		return true;
	}
};

// need a way for this to be const

struct pool_iterator : allocator_iterator
{
	std::vector<linear_iterator> m_pages;
	std::vector<linear_iterator>::iterator m_current;

	pool_iterator()
	{
		m_current = m_pages.begin();
	}

	pool_iterator(const pool_allocator& alloc)
	{
		m_pages.reserve(alloc.m_pages.size());
		for (const auto [_, page] : alloc.m_pages)
		{
			linear_iterator itr = linear_iterator(*page);
			assert(itr.more());
			m_pages.push_back(itr);
		}

		m_current = m_pages.begin();
	}

	char* get_bytes() const override
	{
		return m_current->get_bytes();
	}

	bool more() const override
	{
		return m_current != m_pages.end() && m_current->more();
	}

	void next() override
	{
		m_current->next();
		if (!m_current->more()) ++m_current;
	}
};