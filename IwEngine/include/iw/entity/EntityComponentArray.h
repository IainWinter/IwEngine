#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace IwEntity {
	class IWENTITY_API EntityComponentArray {
	public:
		using ChunkListVec = std::vector<ChunkList::iterator>;

		// !!! THIS MODIFIES OUTER CONTAINER :c !!!
		class IWENTITY_API iterator {
		private:
			ChunkListVec m_itrs;
			ChunkListVec m_ends;
			size_t m_index;

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
				const ChunkListVec& begins,
				const ChunkListVec& ends,
				size_t itrIndex);
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
