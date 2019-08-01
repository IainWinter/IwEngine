#pragma once

#include "IwEntity.h"
#include "EntityArray.h"
#include "iw/util/set/sparse_set.h"
#include <list>

namespace IwEntity {
	class IComponentArray {
	public:
		virtual ~IComponentArray() {}

		virtual void DestroyComponent(
			Entity entity) = 0;

		virtual void Sort(
			EntityArray& entities) = 0;

		virtual void UpdateChunk(
			Entity entity,
			Archetype oldArchetype,
			Archetype archetype) = 0;

		virtual bool HasEntity(
			Entity entity) = 0;

		virtual void Clear() = 0;
	};

	template<
		typename _c>
	class ComponentArray
		: public IComponentArray
	{
	public:
		using RawSparseSet = iwutil::sparse_set<Entity>;
		using SparseSet    = iwutil::sparse_set<Entity, _c>;
		using SparseSetItr = typename SparseSet::iterator;

		struct Chunk {
			using Value = typename SparseSetItr::value_type;

			Archetype Archetype;
			SparseSetItr Begin;
			SparseSetItr End;

			Chunk(
				IwEntity::Archetype& archetype,
				SparseSetItr begin,
				SparseSetItr end)
				: Archetype(archetype)
				, Begin(begin)
				, End(end)
			{}
		};

		using ChunkList    = std::list<Chunk>;
		using ChunkListItr = typename ChunkList::iterator;

	private:
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

			//No Chunks
			if (m_chunks.size() == 0) {
				m_chunks.emplace_back(
					archetype, m_set.begin(), m_set.end());
			}

			else {
				Chunk& chunk = m_chunks.back();
				//Last group has same archetype
				if (chunk.Archetype == archetype) {
					chunk.End = m_set.end();
				}

				else {
					m_chunks.emplace_back(
						archetype, m_set.end() - 1, m_set.end());
				}
			}

			return m_set.at(entity);
		}

		void DestroyComponent(
			Entity entity) override
		{
			auto begin = m_chunks.begin();
			auto end   = m_chunks.end();
			for (auto chunk = begin; chunk != end; chunk++) {
				if (    m_set.at_index(entity) >= chunk->Begin.index()
					&& m_set.at_index(entity) <  chunk->End.index())
				{
					chunk->End--;
					if (chunk->Begin == chunk->End) {
						chunk = m_chunks.erase(chunk);
					}

					break;
				}
			}

			m_set.erase(entity);
		}

		_c* GetComponent(
			Entity entity)
		{
			return &m_set.at(entity);
		}

		void Sort(
			EntityArray& entities) override
		{
			m_set.sort(entities);
			m_chunks.clear();

			auto chunk   = m_chunks.begin();
			auto lastEnd = m_set.begin();

			// Regen chunks
			Archetype lastArchetype = 0;
			for (auto& entity : (RawSparseSet)m_set) {
				Archetype ea = entities.ArchetypeOf(entity);
				if (lastArchetype != ea) {
					m_chunks.emplace_back(ea, lastEnd, lastEnd + 1);
					chunk = m_chunks.end();
					chunk--;
				}

				else {
					chunk->End++;
				}

				lastEnd++;
				lastArchetype = ea;
			}
		}

		void UpdateChunk(
			Entity entity,
			Archetype archetype,
			Archetype oldArchetype) override
		{
			auto begin = m_chunks.begin();
			auto end   = m_chunks.end();
			bool done  = false;
			for (auto chunk = begin; chunk != end; chunk++) {
				//Entity is at top of chunk
				if (m_set.at_index(entity) == chunk->Begin.index()) {
					//Previous chunk exists
					if (chunk != begin) {
						auto pchunk = chunk;
						pchunk--;

						//Previous chunk is correct
						if (pchunk->Archetype == archetype) {
							pchunk->End++;
						}

						else {
							CreateChunkBefore(chunk, archetype);
						}
					}

					else {
						CreateChunkBefore(chunk, archetype);
					}

					chunk->Begin++;
					done = true;
				}

				//Entity is at bottom of chunk
				else if (m_set.at_index(entity)
					 == chunk->End.index() - 1)
				{
					//Next chunk exists
					auto nchunk = chunk;
					nchunk++;

					if (nchunk != end) {
						//Next chunk is correct
						if (nchunk->Archetype == archetype) {
							nchunk->Begin--;
						}

						else {
							CreateChunkAfter(chunk, archetype);
						}
					}

					else {
						CreateChunkAfter(chunk, archetype);
					}

					chunk->End--;
					done = true;
				}

				//Entity is in middle of chunk
				else if (m_set.at_index(entity) > chunk->Begin.index()
					 && m_set.at_index(entity) < chunk->End.index())
				{
					auto eitr   = m_set.find(entity);
					auto nchunk = chunk;
					nchunk++;

					m_chunks.insert(
						chunk, { oldArchetype, chunk->Begin, eitr });

					m_chunks.insert(
						nchunk, { oldArchetype, eitr + 1, chunk->End });

					chunk->Archetype = archetype;
					chunk->Begin = eitr;
					chunk->End   = eitr + 1;

					done = true;
				}

				if (done) {
					//If chunk is empty
					if (chunk->Begin == chunk->End) {
						chunk = m_chunks.erase(chunk);
						chunk--;

						//Combine chunks if same archetype
						auto nchunk = chunk;
						nchunk++;
						if (nchunk != end) {
							if (chunk->Archetype == nchunk->Archetype) {
								chunk->End = nchunk->End;
								m_chunks.erase(nchunk);
							}
						}
					}

					break;
				}
			}
		}

		bool HasEntity(
			Entity entity) override
		{
			return m_set.contains(entity);
		}

		void Clear() override {
			m_set.clear();
			m_chunks.clear();
		}

		ChunkListItr begin() {
			return m_chunks.begin();
		}

		ChunkListItr end() {
			return m_chunks.end();
		}
	private:
		void CreateChunkBefore(
			const ChunkListItr& chunk,
			Archetype archetype)
		{
			m_chunks.insert(
				chunk, { archetype, chunk->Begin, chunk->Begin + 1 });
		}

		void CreateChunkAfter(
			const ChunkListItr& chunk,
			Archetype archetype)
		{
			ChunkListItr nchunk = chunk;
			nchunk++;

			m_chunks.insert(
				nchunk, { archetype, chunk->End - 1, chunk->End });
		}
	};
}
