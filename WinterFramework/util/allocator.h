#pragma once

#include <assert.h>

struct allocator 
{
	virtual char* alloc_bytes(size_t size) = 0;
	virtual void free_bytes(void* address, size_t size) = 0;
	virtual	bool has_space(size_t size) const = 0;
	virtual bool has_allocated(void* address) const = 0;
	virtual bool contains(void* address) const = 0;

	// allows for setting a block size to allocate fixed size byte arrays
	size_t m_block_size = 0;

	template<typename _t>
	_t* alloc(size_t count)
	{
		char* ptr = alloc_bytes(sizeof(_t) * count);
		new (ptr) _t();
		return (_t*)ptr;
	}

	template<typename _t>
	void free(_t* address, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			(address + i)->~_t();
		}

		free_bytes(address, sizeof(_t) * count);
	}

	char* alloc_block()
	{
		assert(m_block_size > 0 && "Tried to alloc block without setting size");
		return alloc_bytes(m_block_size);
	}

	void free_block(void* address)
	{
		assert(m_block_size > 0 && "Tried to free block without setting size");
		free_bytes(address, m_block_size);
	}
};

struct allocator_iterator
{
	virtual char* get_bytes() const = 0;
	virtual bool more() const = 0;
	virtual void next() = 0;

	template<typename _t>
	_t* get() const { return (_t*)get_bytes(); }
};