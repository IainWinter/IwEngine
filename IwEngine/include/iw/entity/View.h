#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include "iw/util/tuple/index.h"
#include "iw/util/tuple/foreach.h"
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
				_cs& ... components)
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
			using ChunkListItr  = typename ComponentArray<_c>::ChunkListItr;
			using ChunkListItrs = std::tuple<ChunkListItr<_cs>...>;

			template<
				typename _c>
			using SparseSetItr  = typename ComponentArray<_c>::SparseSetItr;
			using SparseSetItrs = std::tuple<SparseSetItr<_cs>...>;

			struct Increment {
				template<
					typename _c,
					typename _i>
				void operator()(
					_c&& chunk,
					_i&& itr,
					const Archetype& archetype)
				{
					if (itr < chunk->End) {
						itr++;
					}

					else {
						do {
							chunk++;
						} while (!Similar(chunk->Archetype, archetype));

						itr = chunk->Begin;
					}
				}
			};

			static constexpr std::size_t ComponentCount = sizeof...(_cs);

			Archetype     m_archetype;
			ChunkListItrs m_chunks;
			SparseSetItrs m_itrs;

		public:
			Iterator(
				Archetype archetype,
				ChunkListItr<_cs>... chunks)
				: m_archetype(archetype)
				, m_chunks(chunks...)
				, m_itrs(chunks->Begin...)
			{}

			bool operator!=(
				const Iterator& itr)
			{
				return m_chunks != itr.m_chunks
				    && m_itrs   != itr.m_itrs;
			}

			Iterator& operator++() {
				iwu::foreach<
					Increment,
					ChunkListItrs,
					SparseSetItrs,
					ComponentCount,
					Archetype>
				(
					m_chunks,
					m_itrs,
					m_archetype
				);

				return *this;
			}
		};

	private:
		Archetype m_archetype;
		Iterator  m_itr;

	public:
		View(
			ComponentArray<_cs>&... components)
			: m_archetype(GetArchetype<_cs...>())
			, m_itr(m_archetype, components.begin()...)
		{}

		Iterator begin() {
			return m_itr;
		}

		Iterator end() {
			return Iterator();
		}
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
