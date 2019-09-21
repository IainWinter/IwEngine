#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace IwEntity {
	class EntityComponentArray {
	public:
		class iterator {

		};
	private:
		std::vector<ChunkList::iterator> m_iterators;

	public:
		EntityComponentArray(
			std::vector<ChunkList::iterator> iterators);
	};
}
