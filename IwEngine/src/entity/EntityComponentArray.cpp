#include "iw/entity/EntityComponentArray.h"

namespace IwEntity {
	EntityComponentArray::EntityComponentArray(
		std::vector<ChunkList::iterator>&& begins,
		std::vector<ChunkList::iterator>&& ends)
		: m_begins(std::forward<std::vector<ChunkList::iterator>>(begins))
		, m_ends(std::forward<std::vector<ChunkList::iterator>>(ends))
	{}
}
