#pragma once

#include "allocator.h"
#include "free_list.h"

struct linear_allocator : allocator
{
public:
	void* m_address;
private:
	free_list<size_t> m_freelist;

public:
	linear_allocator(
		size_t size
	)
		: m_address  (malloc(size))
		, m_freelist (0, size)
	{
		assert(size > 0 && "allocator constructed with invalid size");
		assert(m_address && "allocator failed to alloc new page");
	}

	~linear_allocator()
	{
		::free(m_address);
	}

	const free_list<size_t>& freelist() const { return m_freelist; }
	char* address(size_t offset) const { return (char*)m_address + offset; }
	size_t distance(void* address) const { return size_t((char*)address - (char*)m_address); }

	char* alloc_bytes(size_t size) override
	{
		return address(m_freelist.mark_first(size));
	}

	void free_bytes(void* address, size_t size) override
	{
		m_freelist.unmark(distance(address), size);
	}

	bool has_space(size_t size) const override
	{
		return m_freelist.has_space(size);
	}

	bool has_allocated(void* address) const override
	{
		return contains(address) && m_freelist.is_marked(distance(address));
	}

	bool contains(void* address) const override
	{
		return (char*)address >= (char*)m_address 
			&& (char*)address <  (char*)m_address + m_freelist.m_limit_size; 
	}
};

// This iterator only supports iterating over blocks of memory
// asserts will fail if there is a non-uniform range of memory allocated 

struct linear_allocator_iterator : allocator_iterator
{
	size_t m_block_size;

	char* m_begin;
	char* m_end;
	char* m_current;

	free_list<size_t>::const_iterator m_range_limit_current;
	free_list<size_t>::const_iterator m_range_limit_end;

	linear_allocator_iterator(
		linear_allocator& alloc
	)
		: m_block_size          (alloc.m_block_size)
		, m_begin               (alloc.address(0))
		, m_end                 (alloc.address(alloc.freelist().m_limit_size))
		, m_range_limit_current (alloc.freelist().begin())
		, m_range_limit_end     (alloc.freelist().end())
	{
		assert(m_block_size > 0 && "Iterator needs to set block_size to be able to iterate it");

		m_current = m_begin;
		advance_to_valid();
	}

	char* get_bytes() const override
	{
		assert(m_current < m_end && "past end of iterator");
		return m_current;
	}

	bool more() const override
	{
		assert(m_current <= m_end && "past end of iterator");
		return m_current != m_end;
	}

	void next() override
	{
		m_current += m_block_size;
		advance_to_valid();
	}

	void advance_to_valid()
	{
		if (m_range_limit_current != m_range_limit_end)
		{
			assert(m_range_limit_current->m_begin % m_block_size == 0 && "block size is inconsistent");
		
			if (m_current == m_begin + m_range_limit_current->m_begin)
			{
				m_current += m_range_limit_current->m_size;
				++m_range_limit_current;
			}
		}
	}
};