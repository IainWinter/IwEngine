#pragma once

#include <iterator>
#include <tuple>

namespace iwecs {
	template<typename... _components_t>
	class chunk_iterator {
	public:
		using tuple_t           = std::tuple<_components_t...>;
		using iterator_category = std::chunk_iterator_tag;
		using value_type        = std::remove_cv_t<tuple_t>;
		using difference_type   = std::ptrdiff_t;
		using pointer           = tuple_t*;
		using reference         = tuple_t&;
	private:
		pointer m_ptr;
	
	public:
		chunk_iterator()
			: m_ptr() {}

		chunk_iterator(pointer ptr)
		  : m_ptr(ptr) {}

		chunk_iterator(
			const chunk_iterator& copy)
		  : m_ptr(copy.m_ptr) {}

		chunk_iterator& operator=(
			const chunk_iterator& copy)
		{
			m_ptr = copy.m_ptr;
			return *this;
		}

		chunk_iterator& operator++() {
			++m_ptr;
			return *this;
		}

		chunk_iterator operator++(
			int) 
		{
			chunk_iterator tmp(*this);
			++*this;
			return tmp;
		}

		bool operator==(
			const chunk_iterator& itr) const 
		{
			return m_ptr == itr.m_ptr;
		}

		bool operator!=(
			const chunk_iterator& itr) const
		{
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