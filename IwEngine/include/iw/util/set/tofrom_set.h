#pragma once

#include "iw/util/iwutil.h"
#include "iw/util/type/type_traits.h"
#include <algorithm>
#include <assert.h>
#include <functional>
#include <list>
#include <unordered_map>

namespace iwutil {
    template<
		typename _t,
		typename _u>
	class tofrom_set {
	public:
		template<
			bool _const>
		class iterator_ {
		public:
			using to_type   = std::conditional_t<_const, const _t, _t>;
			using from_type = std::conditional_t<_const, const _u, _u>;
			using pointer   = std::conditional_t<
				_const, const std::pair<_t, _u>*, std::pair<_t, _u>*>;
			using reference = std::conditional_t<
				_const, const std::pair<_t, _u>&, std::pair<_t, _u>&>;
			using iteratorcategory = std::bidirectional_iterator_tag;

		private:
			using iterator_t = std::conditional_t<
				_const,
				typename tofrom_set::const_itr_t,
				typename tofrom_set::itr_t>;

			iterator_t m_itr;

			friend class tofrom_set<_t, _u>;

			iterator_(
				const iterator_t& itr)
				: m_itr(itr)
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
				++m_itr;
				return *this;
			}

			iterator_& operator--() {
				--m_itr;
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
				--* this;
				return itr;
			}

			bool operator==(
				const iterator_& itr) const
			{
				return m_itr == itr.m_itr;
			}

			bool operator!=(
				const iterator_& itr) const
			{
				return !(m_itr == itr.m_itr);
			}

			bool operator>(
				const iterator_& itr) const
			{
				return m_itr > itr.m_itr;
			}

			bool operator<(
				const iterator_& itr) const
			{
				return m_itr < itr.m_itr;
			}

			bool operator>=(
				const iterator_& itr) const
			{
				return !(m_itr < itr.m_itr);
			}

			bool operator<=(
				const iterator_& itr) const
			{
				return !(m_itr > itr.m_itr);
			}

			pointer operator->() {
				return m_itr.operator->();
			}

			reference operator*() {
				return m_itr.operator*();
			}
		};
	private:
		using pair_t       = std::pair<_t, _u>;
		using itr_t        = typename std::list<pair_t>::iterator;
		using const_itr_t  = typename std::list<pair_t>::const_iterator;
		using iterator       = iterator_<false>;
		using const_iterator = iterator_<true>;

		std::list<pair_t>             m_items;
		std::unordered_map<_t, itr_t> m_tlookup;
		std::unordered_map<_u, itr_t> m_ulookup;

	public:
		/**
		* @brief Inserts an item pair into the set.
		*
		* @warning
		* Inserting a duplicate item into either set results
		* in undefined behavior. An assertion will check for
		* duplicates at runtime if in debug mode.
		*
		* @param valt An item to insert.
		* @param valu An item to insert.
		*/
		void insert(
			const _t& valt,
			const _u& valu)
		{
			assert(!contains(valt) && !contains_from(valu));

			m_items.push_back(std::make_pair(valt, valu));

			auto itr = --m_items.end();
			m_tlookup[valt] = itr;
			m_ulookup[valu] = itr;
		}

		/**
		* @brief Removes items from the set.
		*
		* @warning
		* Removing an item that doesn't exist in either set results
		* in undefined behavior. An assertion will check the sets
		* for the item at runtime if in debug mode.
		*
		* 
		* @param itr The iterator to the item to remove.
		*/
		void erase(
			const_iterator itr)
		{			
			const _t& t = itr->first;
			const _u& u = itr->second;

			assert(contains(t) && contains_from(u));

			m_tlookup.erase(t);
			m_ulookup.erase(u);
			m_items.erase(itr.m_itr);
		}

		/**
		* @brief Clears the set.
		*/
		virtual void clear() {
			m_tlookup.clear();
			m_ulookup.clear();
			m_items  .clear();
		}

		/**
		* @brief Returns an _u item from a _t item.
		*
		* @warning
		* Attempting to get an item that doesn't exist in either
		* set results in undefined behavior. An assertion will
		* check the sets for the item at runtime if in debug mode.
		*
		* @param index The index of the number to get.
		*
		* @return The number at the specified index.
		*/
		const _u& to(
			const _t& val) const
		{
			assert(contains(val));

			return m_tlookup.at(val)->second;
		}

		/**
		* @copydoc logger::to()
		*/
		const _t& from(
			const _u& val) const
		{
			assert(contains_from(val));

			return m_ulookup.at(val)->first;
		}

		/**
		* @brief Finds an item in the set.
		*
		* Returns an iterator to the item or the end of the set.
		*
		* @param val The item to find.
		*
		* @return An iterator to the specified item if it exists.
		*/
		const_iterator find(
			const _t& val) const
		{
			if (contains(val)) {
				return m_tlookup.at(val);
			}

			return end();
		}

		/**
		* @copydoc logger::find()
		*/
		const_iterator find_from(
			const _u& val) const
		{
			if (contains_from(val)) {
				return m_ulookup.at(val);
			}

			return end();
		}

		/**
		* @brief Checks either set for an item.
		*
		* @param val The item to check for.
		*
		* @return True if the set contains the item, false otherwise.
		*/
		bool contains(
			const _t& val) const
		{
			return m_tlookup.find(val) != m_tlookup.end();
		}

		/**
		* @copydoc logger::contains()
		*/
		bool contains_from(
			const _u& val) const
		{
			return m_ulookup.find(val) != m_ulookup.end();
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
		size_t size() const {
			return m_direct.size();
		}

		/**
		* @brief Returns an iterator to the beginning.
		*
		* @return An iterator to the first item of the set.
		*/
		iterator begin() {
			return iterator(m_items.begin());
		}

		/**
		* @brief Returns an iterator to the end.
		*
		* @return An iterator to the item after the end of the set.
		*/
		iterator end() {
			return iterator(m_items.end());
		}

		/**
		* @brief Returns an iterator to the beginning.
		*
		* @return An iterator to the first item of the set.
		*/
		const_iterator begin() const {
			return const_iterator(m_items.begin());
		}

		/**
		* @brief Returns an iterator to the end.
		*
		* @return An iterator to the item after the end of the set.
		*/
		const_iterator end() const {
			return const_iterator(m_items.end());
		}
	};
}
