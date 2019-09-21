#include "iw/entity/EntityComponentArray.h"

namespace IwEntity {
	EntityComponentArray::EntityComponentArray(
		std::vector<ChunkList::iterator> iterators)
		: m_iterators(iterators)
	{}
}
