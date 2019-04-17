#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include <tuple>

namespace IwEntity {
	template<
		typename... _cs>
	class View {
	public:
		class Iterator {
		private:
			std::tuple<ComponentArray<_cs>::ChunkListItr...> m_itrs;

		public:
			Iterator(
				ComponentArray<_cs>::ChunkListItr... itrs)
				: m_itrs(itrs)
			{}
		};

	private:
		Archetype archetype;
		Iterator  begin;
		Iterator  end;

	public:
		View(
			typename ComponentArray<_cs>&... components)
			: archetype(GetArchetype<_cs...>())
			, begin(components.begin()...)
			, end(components.end()...)
		{}
	};
}

//namespace IwEntity2 {
//	template<
//		typename... _cs>
//	class View {
//	public:
//		class ComponentData {
//		private:
//			using tuple_t = std::tuple<_cs&...>;
//
//			tuple_t m_components;
//
//		public:
//			ComponentData(
//				_cs&... components)
//				: m_components(components...)
//			{}
//
//			template<
//				typename _c>
//			_c& GetComponent() {
//				return std::get<iwu::index<_c&, tuple_t>::value>(
//					m_components);
//			}
//		};
//
//		class Iterator {
//		private:
//			template<
//				typename _c>
//			using ComponentItr = typename iwu::sparse_set<Entity, _c>
//				::iterator;
//
//			using ComponentItrs = std::tuple<ComponentItr<_cs>...>;
//
//			static constexpr std::size_t ComponentCount = sizeof...(_cs);
//
//			ComponentItrs m_itrs;
//
//		public:
//			Iterator(
//				ComponentItr<_cs>& ... itrs)
//				: m_itrs(itrs...)
//			{}
//
//			bool operator!=(const Iterator& itr) {
//				return m_itrs != itr.m_itrs;
//			}
//
//			Iterator& operator++() {
//				iwu::foreach<
//					functors::increment,
//					ComponentItrs,
//					ComponentCount>
//				(
//					m_itrs
//				);
//
//				return *this;
//			}
//
//			ComponentData operator*() {
//				return iwu::geteach<
//					functors::reference,
//					ComponentItrs,
//					ComponentData,
//					ComponentCount>
//				(
//					m_itrs
//				);
//			}
//		};
//	private:
//		Iterator m_begin;
//		Iterator m_end;
//
//	public:
//		View(
//			typename ComponentArray<_cs>::ChunkList&... chunks)
//			: m_begin(begin...)
//		{}
//
//		Iterator begin() {
//			return m_begin;
//		}
//
//		Iterator end() {
//			return m_end;
//		}
//	};
//}
