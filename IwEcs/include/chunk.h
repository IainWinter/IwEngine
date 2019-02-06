#pragma once

#include "iwecs.h"
#include <tuple>

namespace iwecs {
	class ichunk {
	public:
		virtual ~ichunk() {}
	};

	template<typename... T>
	class chunk : public ichunk {
	private:
		using tup_t = std::tuple<T...>;

		tup_t* m_memory;
		tup_t* m_free;
		tup_t* m_end;
		unsigned int m_size;
		unsigned int m_count;
	public:
		chunk(int count)
		  : m_memory(new tup_t[count]),
			m_free(m_memory),
			m_end(m_memory + count),
			m_size(count * sizeof(tup_t)), 
			m_count(0)
		{}

		~chunk() {
			delete[] m_memory;
		}

		bool alloc(T&&... data) {
			if (m_free != m_end) {
				*m_free = std::make_tuple(std::move(data)...);
				m_free++;
				m_count++;

				return true;
			}

			return false;
		}

		bool alloc(const T&... data) {
			if (m_free != m_end) {
				*m_free = std::make_tuple(data...);
				m_free++;
				m_count++;

				return true;
			}

			return false;
		}

		void dealloc() {

		}
	};
}