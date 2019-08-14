#pragma once

#include "IwEntity.h"
#include "ArchetypeData.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/util/set/sparse_set.h"

namespace IwEntity2 {
	class ComponentArray {
	private:
		template<
			bool _const>
		class iterator_ {
		private:
			using iterator__ = typename iwu::sparse_set<Entity, void*>::iterator_<_const>;
			using pointer    = typename iterator__::pointer;
			using reference  = typename iterator__::reference;
			using difference_type   = typename iterator__::difference_type;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator__ m_itr;
			const ArchetypeData m_archetype;

			friend class ComponentArray;

			iterator_(
				iterator__&& itr,
				const ArchetypeData* archetype)
				: m_itr(std::move(itr))
				, m_archetype(*archetype)
			{}
		public:
			iterator_() = default;

			iterator_(
				const iterator_& copy) = default;

			iterator_(
				iterator_&& copy) noexcept = default;

			~iterator_() = default;

			iterator_& operator=(
				const iterator_& copy) = default;

			iterator_& operator=(
				iterator_&& copy) noexcept = default;

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
				--*this;
				return itr;
			}

			iterator_& operator+=(
				const difference_type& value)
			{
				m_itr += value;
				return *this;
			}

			iterator_& operator-=(
				const difference_type& value)
			{
				m_itr -= value;
				return *this;
			}

			iterator_ operator+(
				const difference_type& dif) const
			{
				return m_itr + dif;
			}

			iterator_ operator-(
				const difference_type& dif) const
			{
				return m_itr + dif;
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

			pointer operator->() {
				return m_itr.operator->();
			}

			reference operator*() {
				return *operator->();
			}

			Entity entity() const {
				return m_itr.sparse_index();
			}

			const ArchetypeData& Archetype() const {
				return m_archetype;
			}
		};
	public:

		using Iterator      = iterator_<false>;
		using ConstIterator = iterator_<true>;
	private:
		iwu::pool_allocator            m_pool;
		const ArchetypeData&           m_archetype;
		iwu::sparse_set<Entity, void*> m_entities;

	public:
		ComponentArray(
			size_t pageSize,
			const ArchetypeData& archetypeData);

		void* CreateComponents(
			Entity entity);

		bool DestroyComponents(
			Entity entity);

		void* GetComponents(
			Entity entity);

		void* CopyInto(
			ComponentArray& source,
			Entity entity);

		Iterator begin();

		Iterator end();

		ConstIterator begin() const;

		ConstIterator end() const;

		inline const ArchetypeData& Archetype() const {
			return m_archetype;
		}
	};
}
