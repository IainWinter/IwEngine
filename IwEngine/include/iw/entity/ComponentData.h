#pragma once

#include "IwEntity.h"
#include "iw/util/set/sparse_set.h"
#include <list>

namespace IwEntity {
	class IComponentData {
	public:
		virtual ~IComponentData() {}

		virtual void UpdateChunk(
			Entity entity,
			Archetype archetype) = 0;
	};

	template<
		typename _c>
	class ComponentData
		: public IComponentData
	{
	public:
		using SparseSet = iwutil::sparse_set<Entity, _c>;

		struct Chunk {
			Archetype Archetype;
			typename SparseSet::iterator Begin;
			typename SparseSet::iterator End;
		};


	private:
		using ChunkList    = std::list<Chunk>;
		using ChunkListItr = typename ChunkList::iterator;

		SparseSet m_set;
		ChunkList m_chunks;

	public:
		template<
			typename... _args>
		_c& CreateComponent(
			Entity entity,
			Archetype archetype,
			_args&&... args)
		{
			m_set.emplace(entity, std::forward<_args>(args)...);

			if (m_chunks.size() == 0) {
				m_chunks.push_back({ archetype, m_set.begin(), m_set.end() });
			}

			else {
				Chunk& chunk = m_chunks.back();
				if (chunk.Archetype == archetype) {
					chunk.End = m_set.end();
				}

				else {
					m_chunks.push_back({ archetype, m_set.end() - 1, m_set.end() });
				}
			}

			return m_set.at(entity);
		}

		void UpdateChunk(
			Entity entity,
			Archetype archetype) override
		{
			auto begin = m_chunks.begin();
			auto end   = m_chunks.end();
			auto itr   = begin;
			for (; itr != end; itr++) {
				//Entity is at top of chunk
				if (entity == itr->Begin.index()) {
					//Previous chunk exists
					if (itr != begin) {
						itr--;
						//Previous chunk is correct
						if (itr->Archetype == archetype) {
							itr->End++;
							itr++;
							itr->Begin++;
						}

						else {
							itr++;
							CreateChunkBefore(itr, archetype);
						}
					}

					else {
						CreateChunkBefore(itr, archetype);
					}

					break;
				}

				//Entity is at bottom of chunk
				else if (m_set.at_index(entity) == itr->End.index() - 1) {
					//Next chunk exists
					itr++;
					if (itr != end) {
						//Next chunk is correct
						if (itr->Archetype == archetype) {
							itr->Begin--;
							itr--;
							itr->End--;
						}

						else {
							itr--;
							CreateChunkAfter(itr, archetype);
						}
					}

					else {
						CreateChunkAfter(itr, archetype);
					}

					break;
				}

				else if (entity > itr->Begin.index()
					 && entity < itr->End.index())
				{
					break;
				}
			}
		}
	private:
		void CreateChunkBefore(
			ChunkListItr& itr,
			Archetype archetype)
		{
			auto newitr = m_chunks.insert(itr, { archetype, itr->Begin, itr->Begin + 1 });
			itr->Begin++;
			if (itr->Begin == itr->End) {
				m_chunks.erase(itr);
				itr = newitr;
			}
		}

		void CreateChunkAfter(
			ChunkListItr& itr,
			Archetype archetype)
		{
			ChunkListItr tmp = itr;
			tmp--;

			itr = m_chunks.insert(itr, { archetype, tmp->End - 1, tmp->End });

			//Remove if replacing
			tmp->End--;
			if (tmp->Begin == tmp->End) {
				m_chunks.erase(tmp);
			}

			//Check if need to combine
			tmp = itr;
			tmp--;
			if (tmp->Archetype == itr->Archetype) {
				itr->Begin = tmp->Begin;
				m_chunks.erase(tmp);
			}
		}
	};
}
