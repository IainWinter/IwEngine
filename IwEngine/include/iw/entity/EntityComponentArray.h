#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

namespace iw {
namespace ECS {
	class EntityComponentArray {
	public:
		using ChunkListVec = std::vector<ChunkList::iterator>;

		class iterator {
		private:
			ChunkListVec m_itrs;
			ChunkListVec m_ends;
			size_t m_index;

		public:
			IWENTITY_API
			iterator& operator++();

			IWENTITY_API
			bool operator==(
				const iterator& itr);

			IWENTITY_API
			bool operator!=(
				const iterator& itr);

			IWENTITY_API
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
		IWENTITY_API
		EntityComponentArray(
			ChunkListVec&& begins,
			ChunkListVec&& ends);

		IWENTITY_API
		iterator begin();

		IWENTITY_API
		iterator end();
	};
}

	using namespace ECS;
}
