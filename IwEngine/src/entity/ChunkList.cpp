#include "iw/entity/ChunkList.h"
#include <assert.h>

namespace IwEntity {
	ChunkList::ChunkList(
		iwu::ref<Archetype> archetype,
		size_t chunkSize)
		: m_current(nullptr)
		, m_count(0)
		, m_archetype(archetype)
		, m_chunkSize(chunkSize)
		, m_chunkCapacity(GetChunkCapacity(archetype))
	{}

	void ChunkList::ReserveComponents(
		iwu::ref<Entity> entity)
	{
		Entity* ent = FindOrCreateChunk()->ReserveComponents();
		*ent = *entity;

		m_count++;
	}

	bool ChunkList::FreeComponents(
		iwu::ref<Entity> entity)
	{
		Chunk* chunk = FindChunk(entity->Index);
		if (chunk) {
			chunk->FreeComponents();

			//If chunk is empty free it
			if (chunk->Count == 0) {
				if (chunk->Previous) {
					chunk->Previous->Next = chunk->Next;
				}

				else {
					m_current = nullptr;
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

	Chunk* ChunkList::FindOrCreateChunk() {
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

		return chunk;
	}

	size_t ChunkList::GetChunkCapacity(
		iwu::ref<Archetype> archetype)
	{
		size_t archetypeSize = archetype->Size + sizeof(Entity);
		size_t bufSize       = m_chunkSize - sizeof(Chunk);
		size_t padSize       = bufSize % archetypeSize;

		return (bufSize - padSize) / archetypeSize;
	}

	char* ChunkList::GetChunkStream(
		Chunk* chunk,
		const ArchetypeLayout& layout)
	{
		size_t offset = layout.Offset + sizeof(Entity);
		return chunk->Buffer + offset * m_chunkCapacity;
	}
	
	char* ChunkList::GetComponentData(
		Chunk* chunk,
		const ArchetypeLayout& layout,
		size_t index)
	{
		return GetChunkStream(chunk, layout)
			+ layout.Component->Size
			* (index - chunk->EntityIndex);
	}
}
