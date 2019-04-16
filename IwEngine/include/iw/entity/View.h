#pragma once

#include "IwEntity.h"
#include "iw/util/set/sparse_set.h"
#include "iw/util/tuple/foreach.h"
#include "iw/util/tuple/index.h"
#include "iw/util/type/family.h"
#include <tuple>

namespace IwEntity {
	template<
		typename... _cs>
	class View {
	public:
		class ComponentData {
		private:
			using tuple_t = std::tuple<_cs&...>;

			tuple_t m_components;

		public:
			ComponentData(
				_cs&... components)
				: m_components(components...)
			{}

			template<
				typename _c>
			_c& GetComponent() {
				return std::get<iwu::index<_c&, tuple_t>::value>(
					m_components);
			}
		};

		class Iterator {
		private:
			template<
				typename _c>
			using ComponentItr = typename iwu::sparse_set<Entity, _c>
				::iterator;

			using ComponentItrs = std::tuple<ComponentItr<_cs>...>;

			static constexpr std::size_t ComponentCount = sizeof...(_cs);

			ComponentItrs m_itrs;

		public:
			Iterator(
				ComponentItr<_cs>& ... itrs)
				: m_itrs(itrs...)
			{}

			bool operator!=(const Iterator& itr) {
				return m_itrs != itr.m_itrs;
			}

			Iterator& operator++() {
				iwu::foreach<
					functors::increment,
					ComponentItrs,
					ComponentCount>
				(
					m_itrs
				);

				return *this;
			}

			ComponentData operator*() {
				return iwu::geteach<
					functors::reference,
					ComponentItrs,
					ComponentData,
					ComponentCount>
				(
					m_itrs
				);
			}
		};
	private:
		template<
			typename _c>
		using ComponentSet = iwu::sparse_set<Entity, _c>;

		template<
			typename _c>
		using ComponentItr = typename iwu::sparse_set<Entity, _c>::iterator;

		Iterator m_begin;
		Iterator m_end;

	public:
		View(
			ComponentItr<_cs>&... begin,
			ComponentItr<_cs>&... end)
			: m_begin(begin...)
			, m_end(end...)
		{}

		Iterator begin() {
			return m_begin;
		}

		Iterator end() {
			return m_end;
		}
	};
}
