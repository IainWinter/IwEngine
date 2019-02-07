#pragma once

#include <vector>
#include <assert.h>

namespace iwu {
	template<typename...>
	class sparse_set;


	template<typename _t>
	class sparse_set<_t> {
	public:
		class iterator {
		public:
			using index_type        = _t;
			using direct_type       = std::vector<index_type>;
			using iterator_category = std::random_access_iterator_tag;
			using value_type        = std::remove_cv_t<index_type>;
			using difference_type   = std::ptrdiff_t; //Should be related to _t
			using pointer           = index_type*;
			using reference         = index_type&;

		private:
			index_type m_index;
			direct_type* m_direct;

			friend class sparse_set<_t, _diff_t>;

			iterator(
				index_type index,
				direct_type* direct)
				: m_index(index)
				, m_direct(direct) {}

		public:
			iterator() = default;

			iterator(
				const iterator& copy) = default;

			iterator(
				iterator&& copy) = default;

			~iterator() = default;

			iterator& operator=(
				const iterator& copy) = default;

			iterator& operator=(
				iterator&& copy) = default;

			iterator& operator++() {
				++m_index;
				return *this;
			}

			iterator& operator--() {
				--m_index;
				return *this;
			}

			iterator operator++(
				int)
			{
				iterator itr(*this);
				++*this;
				return itr;
			}

			iterator operator--(
				int)
			{
				iterator itr(*this);
				--*this;
				return itr;
			}

			iterator& operator+=(
				const difference_type& value)
			{
				m_index += value;
				return *this;
			}

			iterator& operator-=(
				const difference_type& value)
			{
				m_index -= value;
				return *this;
			}

			iterator operator+(
				const difference_type& dif)
			{
				return iterator(m_index + dif, m_direct);
			}

			iterator operator-(
				const difference_type& dif)
			{
				return iterator(m_index - dif, m_direct);
			}

			difference_type operator+(
				const iterator& itr)
			{
				return m_index + itr.m_index;
			}

			difference_type operator-(
				const iterator& itr)
			{
				return m_index - itr.m_index;
			}

			bool operator==(
				const iterator& itr) const
			{
				return m_index == itr.m_index;
			}

			bool operator!=(
				const iterator& itr) const
			{
				return !(m_index == itr.m_index);
			}

			bool operator>(
				const iterator& itr) const
			{
				return m_index > itr.m_index;
			}

			bool operator<(
				const iterator& itr) const
			{
				return m_index < itr.m_index;
			}

			bool operator>=(
				const iterator& itr) const
			{
				return !(m_index < itr.m_index);
			}

			bool operator<=(
				const iterator& itr) const
			{
				return !(m_index > itr.m_index);
			}

			pointer operator->() {
				return &(*m_direct)[m_index];
			}

			reference operator*() {
				return *operator->();
			}

			reference operator[](
				const index_type& index)
			{
				return (*m_direct)[m_index + index];
			}
		};
	private:
		std::vector<_t> m_sparse;
		std::vector<_t> m_direct;

		static constexpr _t NULL_VALUE = _t() - 1;

	public:
		~sparse_set() noexcept = default;

		void insert(
			_t x)
		{
			assert(!contains(x));
			if (x > m_sparse.size()) {
				m_sparse.resize(x + 1, NULL_VALUE);
			}

			m_sparse[x] = size();
			m_direct.push_back(x);
		}

		void erase(
			_t x)
		{
			assert(contains(x));
			_t back = m_direct.back();
			_t& swap = m_sparse[x];

			m_sparse[back] = swap;
			m_direct[swap] = back;

			swap = NULL_VALUE;
			m_direct.pop_back();
		}

		//insert
		//erase
		//swap
		//clear
		//emplace

		//at
		//find
		//contains

		//empty
		//size

		//begin
		//end

		_t at(
			_t index)
		{
			return m_direct[index];
		}

		iterator find(
			const _t& val)
		{
			return contains(val) ? end() - at(val) : end();
		}

		bool contains(
			const _t& val)
		{
			return val < m_sparse.size() && m_sparse[val] != NULL_VALUE;
		}

		bool empty() {
			return size() == 0;
		}

		std::size_t size() {
			return m_direct.size();
		}

		iterator begin() {
			return iterator(0, &m_direct);
		}

		iterator end() {
			return iterator(size(), &m_direct);
		}
	private:
	};


	template<
		typename _index_t,
		typename _item_t>
	class sparse_set<_index_t, _item_t>
		: public sparse_set<_index_t>
	{

	};
}