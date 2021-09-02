#include "iw/entity/EntityComponentArray.h"

namespace iw {
namespace ECS {
	using iterator = EntityComponentArray::iterator;

	// I wouldnt be shocked if there wasnt some subtle logic bug somewhere in here

	iterator& iterator::operator++() {
		++m_itrs[Triangles];

		while (Triangles < m_itrs.size() 
			&& m_itrs[Triangles] == m_ends[Triangles])
		{
			++Triangles;
		}

		return *this;
	}

	bool iterator::operator==(
		const iterator& itr)
	{
		if (Triangles == itr.Triangles) {
			if (Triangles < m_itrs.size()) {
				return m_itrs[Triangles] == itr.m_itrs[Triangles];
			}

			return true;
		}

		return false;

		//return Triangles == itr.Triangles 
		//	&& (m_itrs.size() == Triangles || m_itrs[Triangles] == itr.m_itrs[Triangles]);
	}

	bool iterator::operator!=(
		const iterator& itr)
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		return *m_itrs[Triangles];
	}

	iterator::iterator(
		const ChunkListVec& begins,
		const ChunkListVec& ends,
		size_t itrIndex)
		: m_itrs(begins)
		, m_ends(ends)
		, Triangles(itrIndex)
	{
		while (Triangles < m_itrs.size()
			&& m_itrs[Triangles] == m_ends[Triangles])
		{
			++Triangles;
		}
	}

	EntityComponentArray::EntityComponentArray(
		ChunkListVec&& begins,
		ChunkListVec&& ends)
		: m_begins(std::forward<ChunkListVec>(begins))
		, m_ends  (std::forward<ChunkListVec>(ends))
	{}

	iterator EntityComponentArray::begin() {
		return iterator(m_begins, m_ends, 0);
	}

	iterator EntityComponentArray::end() {
		return iterator(m_ends, m_ends, m_begins.size());
	}
}
}
