#pragma once

#include "IwEntity.h"
#include "ChunkList.h"
#include <vector>

#include <functional>

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

		template<typename... _c>
		using func = std::function<void(EntityHandle, _c*...)>;
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

		template<
			typename... _c>
		void Each(
			func<_c...> function)
		{
			for (EntityComponentData& data : *this) {
				callFunction<_c...>(
					function,
					data,
					std::make_index_sequence<sizeof...(_c)>{}
				);
			}
		}

		template<
			typename... _c>
		void First(
			func<_c...> function)
		{
			auto itr = begin();
			if (itr == end()) return;

			EntityComponentData& data = *itr;
			callFunction<_c...>(
				function,
				data,
				std::make_index_sequence<sizeof...(_c)>{}
			);
		}
	private:
		template<
			typename... _c,
			size_t... _i>
		void callFunction(
			func<_c...> func,
			EntityComponentData& data,
			std::index_sequence<_i...>)
		{
			func(data.Handle, (_c*)data.Components.Components[_i]...);
		}
	};

	template<
		typename... _c>
	class ECA
		: public EntityComponentArray
	{
	public:
		using func = std::function<void(EntityHandle, _c*...)>;

		ECA(
			EntityComponentArray&& eca
		)
			: EntityComponentArray(std::forward<EntityComponentArray>(eca))
		{}

		void Each(
			func function)
		{
			EntityComponentArray::Each<_c...>(function);
		}

		void First(
			func function)
		{
			EntityComponentArray::First<_c...>(function);
		}
	};
}

	using namespace ECS;
}
