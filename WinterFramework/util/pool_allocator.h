#pragma once

#include "linear_allocator.h"

struct pool_allocator : allocator
{
	std::vector<linear_allocator*> m_pages;
	size_t m_next_page_size;
	size_t m_next_page_mult;

	pool_allocator()
		: m_next_page_size (0)
		, m_next_page_mult (2)
	{}

	pool_allocator(
		size_t page_expansion
	)
		: m_next_page_size (0)
		, m_next_page_mult (page_expansion)
	{}

	pool_allocator(
		size_t page_size,
		size_t page_expansion
	)
		: m_next_page_size (page_size)
		, m_next_page_mult (page_expansion)
	{}

	~pool_allocator()
	{
		for (linear_allocator* page : m_pages)
		{
			delete page;
		}
	}

	char* alloc_bytes(size_t size) override
	{
		linear_allocator* page = nullptr;
		for (linear_allocator* p : m_pages)
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

			m_pages.push_back(page);

			assert(m_next_page_mult > 0 && "Next page expansion is invalid");
			m_next_page_size *= m_next_page_mult;
		}

		return page->alloc_bytes(size);
	}

	void free_bytes(void* address, size_t size) override
	{
		linear_allocator* page = nullptr;
		for (linear_allocator* p : m_pages)
		{
			if (p->contains(address)) 
			{
				page = p;
				break;
			}
		}

		assert(page && "Address not in pool");

		page->free_bytes(address, size);
		
		if (!page->has_space(1))
		{
			delete page;
			assert(m_next_page_mult > 0 && "Next page expansion is invalid");
			m_next_page_size /= m_next_page_mult;
		}
	}

	bool has_space(size_t size) const override
	{
		for (linear_allocator* page : m_pages) if (page->has_space(size)) return true;
		return false;
	}

	bool has_allocated(void* address) const override
	{
		for (linear_allocator* page : m_pages) if (page->has_allocated(address)) return true;
		return false;
	}

	bool contains(void* address) const override
	{
		for (linear_allocator* page : m_pages) if (page->contains(address)) return true;
		return false;
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
		for (const linear_allocator* page : alloc.m_pages)
		{
			m_pages.push_back(linear_iterator(*page));
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