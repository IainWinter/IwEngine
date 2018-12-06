#pragma once

#include <iostream>
#include "archtype.h"
#include "ct_iteration.h"

namespace iwutil {
	template<typename... _t>
	class pack_input_iterator {
	public:
		using archtype_t        = archtype<_t...>;
		using iterator_category = std::input_iterator_tag;
		using value_type        = std::tuple<std::remove_cv_t<_t>...>;
		using difference_type   = std::ptrdiff_t;
		using pointer           = std::tuple<_t*...>;
		using reference         = std::tuple<_t&...>;
	private:
		pointer m_ptrs;

	public:
		pack_input_iterator()
		  : m_ptrs() {}

		pack_input_iterator(_t*... itrs)
		  : m_ptrs(itrs...) {}

		pack_input_iterator(
			const pack_input_iterator& copy)
		{
			copy_ptrs(copy.m_ptrs);
		}

		pack_input_iterator(
			pack_input_iterator&& copy)
		{
			copy_ptrs(copy.m_ptrs);
		}

		pack_input_iterator& operator=(
			const pack_input_iterator& copy)
		{
			copy_ptrs(copy.m_ptrs);
			return *this;
		}

		pack_input_iterator& operator=(
			pack_input_iterator&& copy)
		{
			copy_ptrs(copy.m_ptrs);
			return *this;
		}

		pack_input_iterator& operator++() {
			increment_itrs();
			return *this;
		}

		pack_input_iterator operator++(int) {
			pack_input_iterator tmp(*this);
			++*this;
			return tmp;
		}

		bool operator==(const pack_input_iterator& itr) const {
			return m_ptrs == itr.m_ptrs;
		}

		bool operator!=(const pack_input_iterator& itr) const {
			return m_ptrs != itr.m_ptrs;
		}

		reference operator*() {
			//return get data ;
		}

		pointer operator->() {
			return m_ptrs;
		}
	private:
		template<
			std::size_t... _tuple_index>
		void increment_itrs(
			std::index_sequence<_tuple_index...>) {
			auto e = {
				(std::get<_tuple_index>(m_ptrs)++, 0)...
			};
		}

		void increment_itrs() {
			increment_itrs(std::make_index_sequence<archtype_t::size>{});
		}

		template<
			std::size_t... _tuple_index>
		void copy_ptrs(
			std::index_sequence<_tuple_index...>,
			pointer&& copy)
		{
			auto e = {
				(std::get<_tuple_index>(m_ptrs)
					= std::get<_tuple_index>(std::forward<pointer>(copy)), 0)...
			};
		}

		void copy_ptrs(
			pointer&& copy)
		{
			copy_ptrs(std::make_index_sequence<archtype_t::size>{},
				std::forward<pointer>(copy));
		}

		void copy_ptrs(
			const pointer& copy) 
		{
			copy_ptrs(std::make_index_sequence<archtype_t::size>{}, copy);
		}
	};
}