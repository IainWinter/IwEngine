#include "iw/entity/ChunkList.h"
#include <assert.h>

namespace IwEntity {
	ChunkList::ChunkList(
		const std::weak_ptr<Archetype2> archetype,
		size_t chunkSize)
		: m_root(nullptr)
		, m_archetype(archetype)
		, m_chunkSize(chunkSize)
		, m_chunkCapacity(GetChunkCapacity(archetype))
	{}

	void ChunkList::ReserveComponents(
		std::shared_ptr<Entity2> entity)
	{
		Chunk* chunk = FindOrCreateChunk();
		entity->ChunkIndex = chunk->ReserveEntity(entity);
		//entity->ComponentData = GetEntityComponentData(chunk, entity->ChunkIndex);
	}

	bool ChunkList::FreeComponents(
		std::shared_ptr<Entity2> entity)
	{
		Chunk* chunk = FindChunk(entity->ChunkIndex);
		if (chunk) {
			return chunk->FreeEntity(entity->ChunkIndex);
		}

		return false;
	}

	Chunk* ChunkList::FindChunk(
		size_t index)
	{
		size_t chunkIndex = index % m_chunkCapacity;
		if (chunkIndex < m_chunks.size()) {
			return m_chunks.at(chunkIndex);
		}

		return nullptr;
	}

	Chunk* ChunkList::CreateChunk() {
		Chunk* chunk = (Chunk*)malloc(m_chunkSize);
		assert(chunk);
		memset(chunk, 0, m_chunkSize);

		m_chunks.push_back(chunk);

		return chunk;
	}

	Chunk* ChunkList::FindOrCreateChunk() {
		Chunk* chunk = nullptr;
		// Chunk doesn't exist
		if (!m_root) {
			chunk = CreateChunk();
			m_root = chunk;
		}

		// Chunk exists
		else {
			// Chunk is full 
			if (ChunkIsFull(m_root)) {
				// Chunk is full but there are others that have space
				if (m_root->Next && !ChunkIsFull(m_root->Next)) {
					// Move root to after the last free chunk
					Chunk* itr = m_root->Next;
					while (itr->Next && !ChunkIsFull(m_root->Next)) {
						itr = itr->Next;
					}

					if (itr) {
						chunk = m_root->Next;
						m_root->Next = itr->Next;
						itr->Next = m_root;
						m_root = chunk;
					}
				}

				// Every chunk is full
				else {
					chunk = CreateChunk();
					chunk->Next = m_root;
					m_root = chunk;
				}
			}

			// Chunk is not full
			else {
				chunk = m_root;
			}
		}

		return chunk;
	}

	size_t ChunkList::GetChunkCapacity(
		std::weak_ptr<Archetype2> archetype)
	{
		size_t archetypeSize = archetype.lock()->Size + sizeof(Entity2);
		size_t bufSize       = m_chunkSize - sizeof(Chunk);
		size_t padSize       = bufSize % archetypeSize;

		return (bufSize - padSize) / archetypeSize;
	}

	char* ChunkList::GetChunkStream(
		Chunk* chunk,
		const ArchetypeLayout& layout)
	{
		size_t offset = layout.Offset + sizeof(Entity2);
		return chunk->Buffer + offset * m_chunkCapacity;
	}
}
