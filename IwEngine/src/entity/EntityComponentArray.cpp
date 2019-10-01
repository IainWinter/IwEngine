#include "iw/entity/EntityComponentArray.h"

namespace IwEntity {
	using iterator = EntityComponentArray::iterator;

	iterator& iterator::operator++() {
		++* m_iterator;

		if (*m_iterator == *m_endIterator) {
			++m_iterator;
			++m_endIterator;
		}

		return *this;
	}

	bool iterator::operator==(
		const iterator& itr)
	{
		return this->m_iterator == itr.m_iterator;
	}

	bool iterator::operator!=(
		const iterator& itr)
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		return **m_iterator;
	}

	iterator::iterator(
		const ChunkListVecItr& begin,
		const ChunkListVecItr& end)
		: m_iterator(begin)
		, m_endIterator(end)
	{}

	EntityComponentArray::EntityComponentArray(
		ChunkListVec&& begins,
		ChunkListVec&& ends)
		: m_begins(std::forward<ChunkListVec>(begins))
		, m_ends(std::forward<ChunkListVec>(ends))
	{}

	iterator EntityComponentArray::begin() {
		return iterator(m_begins.begin(), m_ends.begin());
	}

	iterator EntityComponentArray::end() {
		return iterator(m_begins.end(), m_ends.end());
	}
}
