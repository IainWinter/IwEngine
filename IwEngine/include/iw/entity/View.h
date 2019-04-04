#pragma once

#include "IwEntity.h"
#include "iw/util/set/sparse_set.h"
#include <tuple>

namespace IwEntity {
	//template<
	//	typename... _cs>
	//class IWENTITY_API ComponentData {
	//private:
	//	std::tuple<_cs& ...> m_components;

	//public:
	//	ComponentData(_cs&... components)
	//		: m_components(components...)
	//	{}
	//};

	template<
		typename... _cs>
	class View {
	public:
		class Iterator {
		private:
			template<typename _c>
			using ComponentItr =
				typename iwu::sparse_set<Entity, _c>::iterator;

			std::tuple<ComponentItr<_cs>&...> m_itrs;

		public:
			Iterator(iwu::sparse_set<Entity, _cs>::iterator...) {

			}


		};
	private:
		template<typename _c>
		using ComponentSet = iwu::sparse_set<Entity, _c>;

		std::tuple<ComponentSet<_cs>&...> m_components;

	public:
		View(ComponentSet<_cs>&... components)
			: m_components(components...)
		{}

		Iterator begin() {
			return Iterator(m_components.begin()...);
		}

		Iterator end() {
			return Iterator(m_components...);
		}
	};
}
