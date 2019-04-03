#pragma once

#include "iw/util/iwutil.h"
#include "iw/util/type/type_traits.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <functional>

namespace iwutil {
	template<typename...>
	class sparse_set;

	/**
	* @brief Sparse set specialization for supported integer types.
	*
	* Sparse sets are useful when a sparse dataset needs to be 
	* iterated over as efficiently as possible. 
	*
	* @note
	* It should never be assumed that the order of insertion 
	* will be the order of iteration.
	*
	* @tparam _t The type of integer to store.
	*/
	template<
		typename _t>
	class sparse_set<
		_t>
	{
	public:
		class iterator {
		public:
			using index_type        = _t;
			using direct_type       = const std::vector<index_type>;
			using difference_type   = typename type_traits<_t>::difference_type;
			using iterator_category = std::random_access_iterator_tag;
			using value_type        = std::remove_cv_t<index_type>;
			using pointer           = const index_type*;
			using reference         = const index_type&;

		private:
			index_type m_index;
			const direct_type* m_direct;

			friend class sparse_set<index_type>;

			iterator(
				index_type index,
				direct_type* direct)
				: m_index(index)
				, m_direct(direct)
			{}

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
	protected:
		std::vector<_t> m_sparse;
		std::vector<_t> m_direct;

		static constexpr _t EMPTY_VALUE = _t() - 1;

	public:
		/**
		* @brief Inserts a number into the set.
		*
		* @warning 
		* Inserting a duplicate number into the set results
		* in undefined behavior. An assertion will check for 
		* duplicates at at runtime if in debug mode.
		*
		* @param x The number to insert.
		*/
		void emplace(
			_t x)
		{
			assert(!contains(x));
			if (x >= m_sparse.size()) {
				m_sparse.resize(x + 1, EMPTY_VALUE);
			}

			m_sparse[x] = size();
			m_direct.push_back(x);
		}

		/**
		* @brief Removes a number from the set.
		*
		* @warning
		* Removing a number that doesn't exist in the set results
		* in undefined behavior. An assertion will check the set 
		* for the number at runtime if in debug mode.
		*
		* @param x The number to remove.
		*/
		virtual void erase(
			_t x)
		{
			assert(contains(x));
			_t back  = m_direct.back();
			_t& swap = m_sparse[x];

			m_sparse[back] = swap;
			m_direct[swap] = back;

			swap = EMPTY_VALUE;
			m_direct.pop_back();
		}

		/**
		* @brief Swaps two numbers in the set.
		*
		* @warning
		* Attempting to swap two numbers that don't exist in the 
		* set results in undefined behavior. An assertion will 
		* check the set for the two numbers at runtime if in debug mode.
		*
		* @param a The first number to swap.
		* @param b The second number to swap.
		*/
		virtual void swap(
			_t a,
			_t b)
		{
			assert(contains(a) && contains(b));
			_t ai  = m_sparse[a];
			_t& bi = m_sparse[b];

			m_direct[ai] = b;
			m_direct[bi] = a;

			m_sparse[a] = bi;
			bi = ai;
		}

		/**
		* @brief Sorts the sparse set.
		* Time complexity is that of std::sort.
		* End index of '0' will be interpreted as 'size()'
		*
		* @param comparator Comparison function.
		* @param begin Index to begin at.
		* @param end Index to end at.
		*
		*/
		virtual void sort(
			std::function<bool(_t, _t)> comparator
				= [](_t a, _t b) { return a < b; },
			std::size_t begin = 0,
			std::size_t end = 0)
		{
			if (end == 0) {
				end = size();
			}

			std::sort(m_direct.begin() + begin,
				m_direct.begin() + end, comparator);

			_t count = size();
			for (_t i = 0; i < count; i++) {
				m_sparse[m_direct[i]] = i;
			}
		}

		/**
		* @brief Returns the number at an index.
		*
		* @warning
		* attempting to get a number that doesn't exist in the
		* set results in undefined behavior. An assertion will 
		* check the set for the number at runtime if in debug mode.
		*
		* @param index The index of the number to get.
		*
		* @return The number at the specified index.
		*/
		_t at(
			_t index) const
		{
			assert(contains(index));
			return m_sparse[index];
		}

		/**
		* @brief Finds a number in the set.
		*
		* Returns an iterator to the number or the end of the set.
		*
		* @param val The number to find.
		*
		* @return An iterator to the specified number if it exists.
		*/
		iterator find(
			const _t& val) const
		{
			return contains(val) ? end() - at(val) : end();
		}

		/**
		* @brief Checks the set for a number.
		*
		* @param val The number to check for.
		*
		* @return True if the set contains the number, false otherwise.
		*/
		bool contains(
			const _t& val) const
		{
			return 0 <= val && val < m_sparse.size() && m_sparse[val] != EMPTY_VALUE;
		}

		_t map(
			const _t& index)
		{
			return m_direct[index];
		}

		/**
		* @brief Clears the set.
		*/
		virtual void clear() {
			m_direct.clear();
			m_sparse.clear();
		}

		/**
		* @brief Checks if the set is empty.
		*
		* @return True if the set is empty, false otherwise.
		*/
		bool empty() const {
			return size() == 0;
		}

		/**
		* @brief Returns the size of the set.
		*
		* @return The size of the set.
		*/
		std::size_t size() const {
			return m_direct.size();
		}

		/**
		* @brief Returns an iterator to the beginning.
		*
		* @return An iterator to the first element of the set.
		*/
		iterator begin() const {
			return iterator(0, &m_direct);
		}

		/**
		* @brief Returns an iterator to the end.
		*
		* @return An iterator to the element after the end of the set.
		*/
		iterator end() const {
			return iterator(size(), &m_direct);
		}
	};

	/**
	* @brief Sparse set specialization for supported integer types used
	* as an index to a dense array of items.
	*
	* Sparse sets are useful when a sparse dataset needs to be
	* iterated over as efficiently as possible. This specialization
	* stores an additional array of items in a dense array.
	*
	* @note
	* It should never be assumed that the order of insertion
	* will be the order of iteration.
	*
	* @tparam _index_t The type of integer to use as an index.
	* @tparam _item_t  The type of item to store.
	*/
	template<
		typename _index_t,
		typename _item_t>
	class sparse_set<
		_index_t,
		_item_t>
		: public sparse_set<_index_t>
	{
	public:
		template<
			bool _const>
		class iterator_ {
			public:
				using index_type = _index_t;
				using item_type         
					= std::conditional_t<_const, const _item_t, _item_t>;
				using difference_type
					= typename type_traits<index_type>::difference_type;
				using direct_type       = std::vector<item_type>;
				using iterator_category = std::random_access_iterator_tag;
				using value_type        = std::remove_cv_t<item_type>;
				using pointer           = item_type*;
				using reference         = item_type&;

			private:
				index_type m_index;
				direct_type* m_direct;

				friend class sparse_set<index_type, item_type>;

				iterator_(
					index_type index,
					direct_type* direct)
					: m_index(index)
					, m_direct(direct)
				{}

			public:
				iterator_() = default;

				iterator_(
					const iterator_& copy) = default;

				iterator_(
					iterator_&& copy) = default;

				~iterator_() = default;

				iterator_& operator=(
					const iterator_& copy) = default;

				iterator_& operator=(
					iterator_&& copy) = default;

				iterator_& operator++() {
					++m_index;
					return *this;
				}

				iterator_& operator--() {
					--m_index;
					return *this;
				}

				iterator_ operator++(
					int)
				{
					iterator_ itr(*this);
					++*this;
					return itr;
				}

				iterator_ operator--(
					int)
				{
					iterator_ itr(*this);
					--*this;
					return itr;
				}

				iterator_& operator+=(
					const difference_type& value)
				{
					m_index += value;
					return *this;
				}

				iterator_& operator-=(
					const difference_type& value)
				{
					m_index -= value;
					return *this;
				}

				iterator_ operator+(
					const difference_type& dif)
				{
					return iterator_(m_index + dif, m_direct);
				}

				iterator_ operator-(
					const difference_type& dif)
				{
					return iterator_(m_index - dif, m_direct);
				}

				difference_type operator+(
					const iterator_& itr)
				{
					return m_index + itr.m_index;
				}

				difference_type operator-(
					const iterator_& itr)
				{
					return m_index - itr.m_index;
				}

				bool operator==(
					const iterator_& itr) const
				{
					return m_index == itr.m_index;
				}

				bool operator!=(
					const iterator_& itr) const
				{
					return !(m_index == itr.m_index);
				}

				bool operator>(
					const iterator_& itr) const
				{
					return m_index > itr.m_index;
				}

				bool operator<(
					const iterator_& itr) const
				{
					return m_index < itr.m_index;
				}

				bool operator>=(
					const iterator_& itr) const
				{
					return !(m_index < itr.m_index);
				}

				bool operator<=(
					const iterator_& itr) const
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

				inline index_type index() {
					return m_index;
				}
		};

		using iterator       = iterator_<false>;
		using const_iterator = iterator_<true>;
	private:
		using base_t = sparse_set<_index_t>;

		std::vector<_item_t> m_items;

	public:
		/**
		* @brief Inserts an item into the set.
		*
		*
		* @warning
		* Inserting at a duplicate index results in undefined
		* behavior. An assertion will check for duplicates
		* at at runtime if in debug mode.
		*
		*
		* @param index The index to insert at.
		* @param item The item to insert.
		*/
		void insert(
			_index_t index,
			_item_t&& item)
		{
			m_items.push_back(std::forward<_item_t>(item));
			base_t::emplace(index);
		}

		/**
		* @brief Constructs an item in place at an index.
		*
		* @note
		* Items are constructed in-place if they are an aggregate
		* type.
		*
		* @warning
		* Inserting at a duplicate index results in undefined 
		* behavior. An assertion will check for duplicates 
		* at at runtime if in debug mode.
		*
		* @tparam _args_t The types of arguments to construct the item.
		*
		* @param index The index to insert at.
		* @param args The arguments to construct the item.
		*/
		template<
			typename... _args_t>
		_item_t& emplace(
			_index_t index,
			_args_t&&... args)
		{
			if constexpr (std::is_aggregate_v<_item_t>) {
				m_items.emplace_back(_item_t{ std::forward<_args_t>(args)... });
			}

			else {
				m_items.emplace_back(std::forward<_args_t>(args)...);
			}

			base_t::emplace(index);

			return m_items.back();
		}

		/**
		* @brief Removes an item from the set.
		*
		* @warning
		* Removing at an index that doesn't exist in the set results
		* in undefined behavior. An assertion will check the set
		* for the index at runtime if in debug mode.
		*
		* @param index The index of the element to remove.
		*/
		void erase(
			_index_t index) override
		{
			_item_t back = std::move(m_items.back());
			m_items[base_t::at(index)] = std::move(back);
			m_items.pop_back();
			base_t::erase(index);
		}

		/**
		* @brief Swaps two items in the set.
		*
		* @warning
		* Attempting to swap two items that don't exist in the
		* set results in undefined behavior. An assertion will
		* check the set for the items at runtime if in debug mode.
		*
		* @param a The index of the first item to swap.
		* @param b The index of the second item to swap.
		*/
		void swap(
			_index_t a,
			_index_t b) override
		{
			_item_t item = std::move(m_items[base_t::at(a)]);
			m_items[base_t::at(a)] = std::move(m_items[base_t::at(b)]);
			m_items[base_t::at(b)] = std::move(item);
			base_t::swap(a, b);
		}

		/**
		* @brief Sorts the sparse set.
		* Time complexity is that of std::sort.
		*
		* @param comparator Comparison function.
		* @param begin Index to begin at.
		* @param end Index to end at.
		*
		*/
		void sort(
			std::function<bool(_index_t, _index_t)> comparator
				= [](_index_t a, _index_t b) { return a < b; },
			std::size_t begin = 0,
			std::size_t end = 0) override 
		{
			std::vector<_index_t> direct(m_direct);
			base_t::sort(comparator, begin, end);

			_index_t indexA = 0;
			std::size_t count = size();

			int i = 0;

			bool sorting = true;
			while (sorting) {
				while (direct[indexA] == m_direct[indexA]) {
					indexA++;
					if (indexA == count) {
						goto sorted;
					}
				}

				i++;

				_index_t indexB = m_sparse[direct[indexA]];

				std::swap(m_items[indexA], m_items[indexB]);
				std::swap(direct[indexA], direct[indexB]);

				continue;
			sorted:
				sorting = false;
			}
		}

		/**
		* @brief Clears the set.
		*/
		void clear() override {
			m_items.clear();
			base_t::clear();
		}

		/**
		* @brief Returns the item at an index.
		*
		* @warning
		* attempting to get an item that doesn't exist in the
		* set results in undefined behavior. An assertion will
		* check the set for the item at runtime if in debug mode.
		*
		* @param index The index of the item to get.
		*
		* @return The item at the specified index.
		*/
		_item_t& at(
			_index_t index)
		{
			return m_items[base_t::at(index)];
		}

		/*! @copydoc at */
		const _item_t& at(
			_index_t index) const
		{
			return m_items[base_t::at(index)];
		}

		_item_t& operator[](
			_index_t index)
		{
			return at(index);
		}

		const _item_t& operator[](
			_index_t index) const
		{
			return at(index);
		}

		/**
		* @brief Finds an item in the set at an index.
		*
		* Returns an iterator to the item or the end of the set.
		*
		* @param index The index of the item to find.
		*
		* @return An iterator to the specified item if it exists.
		*/
		iterator find(
			const _index_t& index)
		{
			return contains(index) ? begin() + base_t::at(index) : end();
		}

		/**
		* @brief Returns an iterator to the beginning.
		*
		* @return An iterator to the first element of the set.
		*/
		iterator begin() {
			return iterator(0, &m_items);
		}

		/**
		* @brief Returns an iterator to the end.
		*
		* @return An iterator to the element after the end of the set.
		*/
		iterator end() {
			return iterator(size(), &m_items);
		}

		/*! @copydoc begin */
		const_iterator begin() const {
			return const_iterator(0, &m_items);
		}

		/*! @copydoc end */
		const_iterator end() const {
			return const_iterator(size(), &m_items);
		}
	};
}
