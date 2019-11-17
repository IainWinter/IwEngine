#include "iw/entity/EntityComponentArray.h"

namespace IW {
	using iterator = EntityComponentArray::iterator;

	iterator& iterator::operator++() {
		++m_itrs[m_index];

		while (m_index < m_itrs.size() 
			&& m_itrs[m_index] == m_ends[m_index])
		{
			++m_index;
		}

		return *this;
	}

	bool iterator::operator==(
		const iterator& itr)
	{
		if (m_index == itr.m_index) {
			if (m_index < m_itrs.size()) {
				return m_itrs[m_index] == itr.m_itrs[m_index];
			}

			return true;
		}

		return false;

		//return m_index == itr.m_index 
		//	&& (m_itrs.size() == m_index || m_itrs[m_index] == itr.m_itrs[m_index]);
	}

	bool iterator::operator!=(
		const iterator& itr)
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		return *m_itrs[m_index];
	}

	iterator::iterator(
		const ChunkListVec& begins,
		const ChunkListVec& ends,
		size_t itrIndex)
		: m_itrs(begins)
		, m_ends(ends)
		, m_index(itrIndex)
	{
		while (m_index < m_itrs.size()
			&& m_itrs[m_index] == m_ends[m_index])
		{
			++m_index;
		}
	}

	EntityComponentArray::EntityComponentArray(
		ChunkListVec&& begins,
		ChunkListVec&& ends)
		: m_begins(std::forward<ChunkListVec>(begins))
		, m_ends(std::forward<ChunkListVec>(ends))
	{}

	iterator EntityComponentArray::begin() {
		return iterator(m_begins, m_ends, 0);
	}

	iterator EntityComponentArray::end() {
		return iterator(m_ends, m_ends, m_begins.size());
	}
}
