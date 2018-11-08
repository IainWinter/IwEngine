#pragma once

#include "iwecs.h"

template<typename T>
class memory_chunk {
private:
	T* m_memory;
	T* m_free;
	T* m_end;
	unsigned int m_size;
	unsigned int m_count;
public:
	memory_chunk(int count)
		: m_size(count * sizeof(T)),
		m_memory(new T[m_size]),
		m_free(m_memory),
		m_end(m_memory + count) {
	}

	~memory_chunk() {
		delete[] m_memory;
	}

	bool alloc(T&& data) {
		if (m_free != m_end) {
			*m_free = T(std::move(data));
			m_free++;
			m_count++;

			return true;
		}

		return false;
	}

	bool alloc(T& data) {
		if (m_free != m_end) {
			*m_free = T(data);
			m_free++;
			m_count++;

			return true;
		}

		return false;
	}

	void dealloc() {

	}
};