#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace IwEntity {
	class IWENTITY_API EntityComponentArray {
	private:
		using ChunkListVec    = std::vector<ChunkList::iterator>;
		using ChunkListVecItr = ChunkListVec::iterator;
	public:
		class IWENTITY_API iterator {
		public:
		private:
			ChunkListVecItr m_iterator;
			ChunkListVecItr m_endIterator;

		public:
			iterator& operator++();

			bool operator==(
				const iterator& itr);

			bool operator!=(
				const iterator& itr);

			EntityComponentData operator*();
		private:
			friend class EntityComponentArray;

			iterator(
				const ChunkListVecItr& begin,
				const ChunkListVecItr& end);
		};
	private:
		ChunkListVec m_begins;
		ChunkListVec m_ends;

	public:
		EntityComponentArray(
			ChunkListVec&& begins,
			ChunkListVec&& ends);

		iterator begin();
		iterator end();
	};
}
