#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace IwEntity {
	class EntityComponentArray {
	private:
		std::vector<ChunkList::iterator> m_iterator;

	public:
		EntityComponentArray(
			std::vector<ChunkList::iterator> iterators);
	};
}
