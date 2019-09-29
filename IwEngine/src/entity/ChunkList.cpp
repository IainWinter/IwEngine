#include "iw/entity/ChunkList.h"
#include <assert.h>

namespace IwEntity {
	ChunkList::ChunkList(
		const iwu::ref<Archetype>& archetype,
		size_t chunkSize)
		: m_current(nullptr)
		, m_count(0)
		, m_archetype(archetype)
		, m_chunkSize(chunkSize)
		, m_chunkCapacity(GetChunkCapacity(archetype))
	{}

	size_t ChunkList::ReserveComponents(
		const Entity& entity)
	{
		Chunk& chunk = FindOrCreateChunk();
		size_t index = chunk.ReserveComponents();

		Entity* entityComponent = chunk.GetEntity(index);
		*entityComponent = entity;

		m_count++;

		return index;
	}

	bool ChunkList::ReinstateComponents(
		const iwu::ref<EntityData>& entityData)
	{
		Chunk* chunk = FindChunk(entityData->ChunkIndex);
		if (chunk) {
			chunk->ReinstateComponents();

			return true;
		}

		return false;
	}

	bool ChunkList::FreeComponents(
		size_t index)
	{
		Chunk* chunk = FindChunk(index);
		if (chunk) {
			chunk->FreeComponents();

			Entity* entityComponent = chunk->GetEntity(index);
			entityComponent->Alive = false;

			//If chunk is empty free it
			if (chunk->Count == 0) {
				if (chunk->Previous) {
					chunk->Previous->Next = chunk->Next;
				}

				else if (chunk->Next) {
					chunk->Next->Previous = chunk->Previous;
				}

				else {
					m_current = nullptr;   //This needs to be looked at, maybe 
					m_chunks.clear(); // might need to delete sooner
					m_chunks.shrink_to_fit();
					m_count = 0;
				}

				free(chunk);
			}

			return true;
		}

		return false;
	}

	ChunkList::iterator ChunkList::begin() {
		if (m_chunks.size() == 0) {
			return iterator(nullptr, 0);
		}

		return iterator(m_chunks.front(), 0);
	}

	ChunkList::iterator ChunkList::end() {
		if (m_chunks.size() == 0) {
			return iterator(nullptr, 0);
		}

		return iterator(m_chunks.back(), m_chunks.back()->Count);
	}

	Chunk* ChunkList::FindChunk(
		size_t index)
	{
		size_t chunkIndex = index / m_chunkCapacity;
		if (chunkIndex < m_chunks.size()) {
			return m_chunks.at(chunkIndex);
		}

		return nullptr;
	}

	Chunk* ChunkList::CreateChunk() {
		Chunk* chunk = (Chunk*)malloc(m_chunkSize);
		assert(chunk);
		memset(chunk, 0, m_chunkSize);

		chunk->EntityIndex = m_count;

		m_chunks.push_back(chunk);

		return chunk;
	}

	Chunk& ChunkList::FindOrCreateChunk() {
		Chunk* chunk = nullptr;
		// Chunk doesn't exist
		if (!m_current) {
			chunk = CreateChunk();
			m_current = chunk;
		}

		// Chunk exists
		else {
			// Chunk is full 
			if (ChunkIsFull(m_current)) {
				//// Chunk is full but there are others that have space
				//if (m_root->Next && !ChunkIsFull(m_root->Next)) {
				//	// Move root to after the last free chunk
				//	Chunk* itr = m_root->Next;
				//	while (itr->Next && !ChunkIsFull(m_root->Next)) {
				//		itr = itr->Next;
				//	}

				//	if (itr) {
				//		chunk = m_root->Next;
				//		m_root->Next = itr->Next;
				//		itr->Next = m_root;
				//		m_root = chunk;
				//	}
				//}

				//// Every chunk is full
				//else {
					chunk = CreateChunk();
					chunk->Previous = m_current;
					m_current->Next = chunk;
					m_current = chunk;
				//}
			}

			// Chunk is not full
			else {
				chunk = m_current;
			}
		}

		return *chunk;
	}

	size_t ChunkList::GetChunkCapacity(
		const iwu::ref<Archetype>& archetype)
	{
		size_t archetypeSize = archetype->Size + sizeof(Entity);
		size_t bufSize       = m_chunkSize - sizeof(Chunk);
		size_t padSize       = bufSize % archetypeSize;

		return (bufSize - padSize) / archetypeSize;
	}
	
	char* ChunkList::GetComponentData(
		Chunk* chunk,
		const ArchetypeLayout& layout,
		size_t index)
	{
		return chunk->GetComponentStream(m_chunkCapacity, layout)
			+ layout.Component->Size
			* (index - chunk->EntityIndex);
	}
}
