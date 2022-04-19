#pragma once

#include <assert.h>

struct allocator 
{
	virtual char* alloc_bytes(size_t size) = 0;
	virtual void free_bytes(void* address, size_t size) = 0;
	
	virtual bool has_space(size_t size) const = 0;
	virtual bool has_allocated(void* address) const = 0;
	
	virtual bool contains(void* address) const = 0;
	virtual bool contains_block(size_t index) const = 0;

	virtual size_t get_block_index(void* address) const = 0;
	virtual void*  get_block_address(size_t index) const = 0;

	virtual size_t capacity() const = 0;

	size_t block_capacity() const
	{
		assert(m_block_size > 0 && "Block size needs to be set to use this");
		return capacity() / m_block_size;
	}

	// allows for setting a block size to allocate fixed size byte arrays
	size_t m_block_size = 0;

	template<typename _t>
	_t* alloc(size_t count = 1)
	{
		char* address = alloc_bytes(sizeof(_t) * count);
		for (size_t i = 0; i < count; i++)
		{
			new (address + sizeof(_t) * i) _t();
		}

		return (_t*)address;
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
