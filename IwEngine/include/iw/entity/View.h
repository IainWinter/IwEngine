#pragma once

#include "IwEntity.h"
#include "iw/util/set/sparse_set.h"
#include <tuple>

namespace IwEntity {
	template<
		typename... _cs>
	class IWENTITY_API View {
	private:
		template<
			typename _c>
		using ComponentItr = iwu::sparse_set<Entity, _c>::iterator;

		std::tuple<ComponentItr<_cs>...> m_itrs;

	public:
		View(ComponentItr<_cs>... itrs)
			: m_itrs(itrs)
		{}
	};
}
