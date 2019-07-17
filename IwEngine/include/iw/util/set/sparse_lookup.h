#pragma once

#include "sparse_set.h"
#include "iw/util/iwutil.h"
#include "iw/util/type/type_traits.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <functional>

namespace iwutil {
	/**
	* @brief
	*
	*
	*
	* @note

	*
	* @tparam _item_t The type of item to store.
	* @tparam _item_u The type of item to store.
	*/
	template<
		typename _item_t,
		typename _item_u>
	class sparse_lookup
	{
	public:
		/*template<
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
				using pointer           = item_type *;
				using reference         = item_type &;

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
					++* this;
					return itr;
				}

				iterator_ operator--(
					int)
				{
					iterator_ itr(*this);
					--* this;
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
		using const_iterator = iterator_<true>;*/
	private:
		std::vector<std::pair<_item_t, _item_t>> m_items;
		std::unordered_map<_item_t, size_t> m_t_lookup;
		std::unordered_map<_item_t, size_t> m_u_lookup;

	public:
		///**
		//* @brief Inserts an item into the set.
		//*
		//*
		//* @warning
		//* Inserting at a duplicate index results in undefined
		//* behavior. An assertion will check for duplicates
		//* at at runtime if in debug mode.
		//*
		//*
		//* @param index The index to insert at.
		//* @param item The item to insert.
		//*/
		//void insert(
		//	_index_t index,
		//	_item_t&& item)
		//{
		//	m_items.push_back(std::forward<_item_t>(item));
		//	base_t::emplace(index);
		//}

		///**
		//* @brief Constructs an item in place at an index.
		//*
		//* @note
		//* Items are constructed in-place if they are an aggregate
		//* type.
		//*
		//* @warning
		//* Inserting at a duplicate index results in undefined
		//* behavior. An assertion will check for duplicates
		//* at at runtime if in debug mode.
		//*
		//* @tparam _args_t The types of arguments to construct the item.
		//*
		//* @param index The index to insert at.
		//* @param args The arguments to construct the item.
		//*/
		//template<
		//	typename... _args_t>
		//	_item_t& emplace(
		//		_index_t index,
		//		_args_t&& ... args)
		//{
		//	if constexpr (std::is_aggregate_v<_item_t>) {
		//		m_items.emplace_back(_item_t{ std::forward<_args_t>(args)... });
		//	}

		//	else {
		//		m_items.emplace_back(std::forward<_args_t>(args)...);
		//	}

		//	base_t::emplace(index);

		//	return m_items.back();
		//}

		///**
		//* @brief Removes an item from the set.
		//*
		//* @warning
		//* Removing at an index that doesn't exist in the set results
		//* in undefined behavior. An assertion will check the set
		//* for the index at runtime if in debug mode.
		//*
		//* @param index The index of the element to remove.
		//*/
		//void erase(
		//	_index_t index) override
		//{
		//	_item_t back = std::move(m_items.back());
		//	m_items[base_t::at(index)] = std::move(back);
		//	m_items.pop_back();
		//	base_t::erase(index);
		//}

		///**
		//* @brief Swaps two items in the set.
		//*
		//* @warning
		//* Attempting to swap two items that don't exist in the
		//* set results in undefined behavior. An assertion will
		//* check the set for the items at runtime if in debug mode.
		//*
		//* @param a The index of the first item to swap.
		//* @param b The index of the second item to swap.
		//*/
		//void swap(
		//	_index_t a,
		//	_index_t b) override
		//{
		//	_item_t item = std::move(m_items[base_t::at(a)]);
		//	m_items[base_t::at(a)] = std::move(m_items[base_t::at(b)]);
		//	m_items[base_t::at(b)] = std::move(item);
		//	base_t::swap(a, b);
		//}

		///**
		//* @brief Sorts the sparse set.
		//* Time complexity is that of std::sort.
		//*
		//* @param comparator Comparison function.
		//* @param begin Index to begin at.
		//* @param end Index to end at.
		//*
		//*/
		//void sort(
		//	std::function<bool(_index_t, _index_t)> comparator
		//	= [](_index_t a, _index_t b) { return a < b; },
		//	std::size_t begin = 0,
		//	std::size_t end = 0) override
		//{
		//	if (size() == 0) {
		//		return;
		//	}

		//	std::vector<_index_t> direct(m_direct);
		//	base_t::sort(comparator, begin, end);

		//	_index_t indexA = 0;
		//	std::size_t count = size();

		//	int i = 0;

		//	bool sorting = true;
		//	while (sorting) {
		//		while (direct[indexA] == m_direct[indexA]) {
		//			indexA++;
		//			if (indexA == count) {
		//				goto sorted;
		//			}
		//		}

		//		i++;

		//		_index_t indexB = m_sparse[direct[indexA]];

		//		std::swap(m_items[indexA], m_items[indexB]);
		//		std::swap(direct[indexA], direct[indexB]);

		//		continue;
		//	sorted:
		//		sorting = false;
		//	}
		//}

		///**
		//* @brief Clears the set.
		//*/
		//void clear() override {
		//	m_items.clear();
		//	base_t::clear();
		//}

		///**
		//* @brief Returns the item at an index.
		//*
		//* @warning
		//* attempting to get an item that doesn't exist in the
		//* set results in undefined behavior. An assertion will
		//* check the set for the item at runtime if in debug mode.
		//*
		//* @param index The index of the item to get.
		//*
		//* @return The item at the specified index.
		//*/
		//_item_t& at(
		//	_index_t index)
		//{
		//	return m_items[base_t::at(index)];
		//}

		///*! @copydoc at */
		//const _item_t& at(
		//	_index_t index) const
		//{
		//	return m_items[base_t::at(index)];
		//}

		//_item_t& operator[](
		//	_index_t index)
		//{
		//	return at(index);
		//}

		//const _item_t& operator[](
		//	_index_t index) const
		//{
		//	return at(index);
		//}

		///**
		//* @brief Finds an item in the set at an index.
		//*
		//* Returns an iterator to the item or the end of the set.
		//*
		//* @param index The index of the item to find.
		//*
		//* @return An iterator to the specified item if it exists.
		//*/
		//iterator find(
		//	const _index_t& index)
		//{
		//	return contains(index) ? begin() + base_t::at(index) : end();
		//}

		//_index_t at_index(
		//	_index_t index)
		//{
		//	return base_t::at(index);
		//}

		///**
		//* @brief Returns an iterator to the beginning.
		//*
		//* @return An iterator to the first element of the set.
		//*/
		//iterator begin() {
		//	return iterator(0, &m_items);
		//}

		///**
		//* @brief Returns an iterator to the end.
		//*
		//* @return An iterator to the element after the end of the set.
		//*/
		//iterator end() {
		//	return iterator(size(), &m_items);
		//}

		///*! @copydoc begin */
		//const_iterator begin() const {
		//	return const_iterator(0, &m_items);
		//}

		///*! @copydoc end */
		//const_iterator end() const {
		//	return const_iterator(size(), &m_items);
		//}
	};
}
