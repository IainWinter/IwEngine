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
	private:
		template<
			typename _c>
		using ChunkListItr  = typename ComponentArray<_c>::ChunkListItr;
		using ChunkListItrs = std::tuple<ChunkListItr<_cs>...>;

		template<
			typename _c>
		using SparseSetItr  = typename ComponentArray<_c>::SparseSetItr;
		using SparseSetItrs = std::tuple<SparseSetItr<_cs>...>;

		static constexpr std::size_t ComponentCount = sizeof...(_cs);
	public:
		struct ComponentData {
			using Components = std::tuple<_cs& ...>;

			Components m_components;

			ComponentData(
				_cs&... components)
				: m_components(components...)
			{}

			template<
				typename _c>
			_c& GetComponent() {
				return std::get<iwu::index<_c&, Components>::value>(
					m_components);
			}
		};

		class Iterator {
		private:
			struct GetBegin {
				template<
					typename _t>
				SparseSetItr<typename _t::value_type::Value> operator()(
					const _t& begin)
				{
					return begin->Begin;
				}
			};

			struct GetEnd {
				template<
					typename _t>
				SparseSetItr<typename _t::value_type::Value> operator()(
					const _t& end)
				{
					_t last = end;
					--last;
					return last->End;
				}
			};

			struct Increment {
				template<
					typename _c,
					typename _i>
				void operator()(
					_c& chunk,
					const Archetype& archetype,
					const _c& end,
					_i& itr)
				{
					if (chunk != end) {
						++itr;
						if (itr == chunk->End) {
							do {
								++chunk;
							} while (chunk != end
								&& !Similar(chunk->Archetype, archetype));

							if (chunk != end) {
								itr = chunk->Begin;
							}
						}
					}
				}
			};

			const Archetype&     m_archetype;
			const ChunkListItrs& m_begins;
			const ChunkListItrs& m_ends;
			ChunkListItrs m_chunks;
			SparseSetItrs m_itrs;
		public:
			Iterator(
				const Archetype& archetype,
				const ChunkListItrs& begin,
				const ChunkListItrs& end,
				bool isBegin)
				: m_archetype(archetype)
				, m_begins(begin)
				, m_ends(end)
				, m_chunks(isBegin ? begin : end)
			{
				if (isBegin) {
					m_itrs = iwu::geteach<
							GetBegin,
							const ChunkListItrs,
							SparseSetItrs,
							ComponentCount>
						(
							begin
						);
				}

				else {
					m_itrs = iwu::geteach<
							GetEnd,
							const ChunkListItrs,
							SparseSetItrs,
							ComponentCount>
						(
							end
						);
				}
			}

			Iterator& operator++() {
				iwu::formatrix<
					Increment,
					ChunkListItrs,
					ComponentCount,
					const Archetype,
					const ChunkListItrs,
					SparseSetItrs>
				(
					m_chunks,
					m_archetype,
					m_ends,
					m_itrs
				);

				return *this;
			}

			bool operator!=(
				const Iterator& other) const
			{
				return !(m_chunks == other.m_chunks)
				    && !(m_itrs == other.m_itrs);
			}

			ComponentData operator*() {
				return iwu::geteach<
					functors::reference,
					SparseSetItrs,
					ComponentData,
					ComponentCount>
				(
					m_itrs
				);
			}
		};

	private:
		const Archetype     m_archetype;
		const ChunkListItrs m_begin;
		const ChunkListItrs m_end;
	public:
		View(
			ComponentArray<_cs>&... components)
			: m_archetype(GetArchetype<_cs...>())
			, m_begin(components.begin()...)
			, m_end(components.end()...)
		{}

		Iterator begin() {
			return Iterator(m_archetype, m_begin, m_end, true);
		}

		Iterator end() {
			return Iterator(m_archetype, m_begin, m_end, false);
		}
	};
}
