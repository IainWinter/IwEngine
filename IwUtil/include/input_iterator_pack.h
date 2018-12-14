#pragma once

#include "archetype.h"
#include "tuple_iteration.h"

namespace iwutil {
	template<typename... _t>
	class input_iterator_pack {
	public:
		using archetype_t        = archetype<_t...>;
		using iterator_category = std::input_iterator_tag;
		using value_type        = std::tuple<std::remove_cv_t<_t>...>;
		using difference_type   = std::ptrdiff_t;
		using pointer           = std::tuple<_t*...>;
		using reference         = std::tuple<_t&...>;
	private:
		pointer m_ptrs;

	public:
		input_iterator_pack()
		  : m_ptrs() {}

		input_iterator_pack(_t*... itrs)
		  : m_ptrs(itrs...) {}

		input_iterator_pack(
			const input_iterator_pack& copy)
		{
			copy_ptrs(copy.m_ptrs);
		}

		input_iterator_pack(
			input_iterator_pack&& copy)
		{
			copy_ptrs(copy.m_ptrs);
		}

		input_iterator_pack& operator=(
			const input_iterator_pack& copy)
		{
			copy_ptrs(copy.m_ptrs);
			return *this;
		}

		input_iterator_pack& operator=(
			input_iterator_pack&& copy)
		{
			copy_ptrs(copy.m_ptrs);
			return *this;
		}

		input_iterator_pack& operator++() {
			increment_itrs();
			return *this;
		}

		input_iterator_pack operator++(
			int)
		{
			input_iterator_pack tmp(*this);
			++*this;
			return tmp;
		}

		bool operator==(
			const input_iterator_pack& itr) const
		{
			return m_ptrs == itr.m_ptrs;
		}

		bool operator!=(
			const input_iterator_pack& itr) const
		{
			return m_ptrs != itr.m_ptrs;
		}

		reference operator*() {
			return reference_itrs();
		}

		pointer operator->() {
			return m_ptrs;
		}
	private:
		void increment_itrs() {
			foreach<
				functors::increment,
				pointer,
				archetype_t::size>
			(
				m_ptrs
			);
		}

		void copy_ptrs(
			const pointer& copy)
		{
			foreach<
				functors::assign,
				pointer,
				pointer,
				archetype_t::size>
			(
				m_ptrs,
				copy
			);
		}

		reference reference_itrs() {
			return geteach<
				functors::reference,
				pointer,
				reference,
				archetype_t::size>
			(
				m_ptrs
			);
		}
	};
}