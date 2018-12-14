#pragma once

#include <iterator>

namespace iwutil {
	template<typename _t>
	class input_iterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type        = std::remove_cv_t<_t>;
		using difference_type   = std::ptrdiff_t;
		using pointer           = _t*;
		using reference         = _t&;
	private:
		pointer m_ptr;
	public:
		input_iterator()
			: m_ptr(0) {
		}

		input_iterator(pointer ptr)
			: m_ptr(ptr) {
		}

		input_iterator(const input_iterator& copy)
			: m_ptr(copy.m_ptr) {
		}

		input_iterator& operator=(const input_iterator& copy) {
			m_ptr = copy.m_ptr;
			return *this;
		}

		input_iterator& operator++() {
			++m_ptr;
			return *this;
		}

		input_iterator operator++(int) {
			input_iterator tmp(*this);
			++*this;
			return tmp;
		}

		bool operator==(const input_iterator& itr) const {
			return m_ptr == itr.m_ptr;
		}

		bool operator!=(const input_iterator& itr) const {
			return m_ptr != itr.m_ptr;
		}

		reference operator*() {
			return *m_ptr;
		}

		pointer operator->() {
			return m_ptr;
		}
	};
}