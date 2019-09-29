#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace IwEntity {
	class EntityComponentArray {
	public:
		class iterator { // move code to cpp
		public:
		private:
			std::vector<ChunkList::iterator>::iterator m_iterator;
			std::vector<ChunkList::iterator>::iterator m_endIterator;

		public:
			iterator& operator++() {
				++*m_iterator;

				if (*m_iterator == *m_endIterator) {
					++m_iterator;
					++m_endIterator;
				}

				return *this;
			}

			bool operator==(
				const iterator& itr)
			{
				return this->m_iterator == itr.m_iterator;
			}

			bool operator!=(
				const iterator& itr)
			{
				return !operator==(itr);
			}

			Entity& operator*() {
				return **m_iterator;
			}
		private:
			friend class EntityComponentArray;

			iterator(
				const std::vector<ChunkList::iterator>::iterator& begin,
				const std::vector<ChunkList::iterator>::iterator& end)
				: m_iterator(begin)
				, m_endIterator(end)
			{}
		};
	private:
		std::vector<ChunkList::iterator> m_begins;
		std::vector<ChunkList::iterator> m_ends;

	public:
		EntityComponentArray(
			std::vector<ChunkList::iterator>&& begins,
			std::vector<ChunkList::iterator>&& ends);

		iterator begin() {
			return iterator(m_begins.begin(), m_ends.begin());
		}

		iterator end() {
			return iterator(m_begins.end(), m_ends.end());
		}
	};
}
