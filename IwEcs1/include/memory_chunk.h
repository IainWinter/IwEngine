#pragma once

#include "iwecs.h"

namespace iwecs {
	class t {
	public:
		void tr();
	};

	class imemory_chunk {
	public:
		imemory_chunk() {}
		virtual ~imemory_chunk() {}

		virtual void alloc() = 0;
	};

	template<typename T>
	class IWECS_API memory_chunk : public imemory_chunk {
	private:
		T* m_memory;
		T* m_free;
		T* m_end;
		unsigned int m_size;
		unsigned int m_count;
	public:
		memory_chunk(int count)
		  : m_size  (count * sizeof(T)),
			m_memory(new T[m_size]),
			m_free  (m_memory),
			m_end   (m_memory + count)
		{}

		~memory_chunk() {
			delete[] m_memory;
		}

		void alloc(T&& data) override {
			if (m_free != m_end) {
				m_free = T(data);
				m_free++;
				m_count++;
			}
		}
	};
}