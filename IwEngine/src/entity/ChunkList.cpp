#include "iw/entity/ChunkList.h"
#include <assert.h>

namespace IwEntity {
	ChunkList::ChunkList(
		iwu::ref<const Archetype2> archetype,
		size_t chunkSize)
		: m_root(nullptr)
		, m_archetype(archetype)
		, m_chunkSize(chunkSize)
		, m_chunkCapacity(GetChunkCapacity(archetype))
	{}

	iwu::ref<ComponentData> ChunkList::ReserveComponents(
		iwu::ref<const Entity2> entity)
	{
		size_t bufSize = sizeof(ComponentData)
			+ sizeof(void*)
			* entity->Archetype->Count;

		ComponentData* buf = (ComponentData*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		Chunk* chunk = FindOrCreateChunk();
		
		buf->ChunkIndex = chunk->ReserveEntity(entity);
		for (size_t i = 0; i < entity->Archetype->Count; i++) {
			buf->Components[i] = GetComponentData(
				chunk, entity->Archetype->Layout[i], buf->ChunkIndex);
		}

		return iwu::ref<ComponentData>(buf, free);
	}

	bool ChunkList::FreeComponents(
		iwu::ref<const Entity2> entity)
	{
		Chunk* chunk = FindChunk(entity->ComponentData->ChunkIndex);
		if (chunk) {
			chunk->FreeEntity(entity->ComponentData->ChunkIndex);

			//If chunk is empty free it
			if (chunk->Count == 0) {
				chunk->Previous->Next = chunk->Next;
				free(chunk);
			}

			return true;
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
					chunk->Next      = m_root;
					m_root->Previous = chunk;
					m_root = chunk;
				//}
			}

			// Chunk is not full
			else {
				chunk = m_root;
			}
		}

		return chunk;
	}

	size_t ChunkList::GetChunkCapacity(
		iwu::ref<const Archetype2> archetype)
	{
		size_t archetypeSize = archetype->Size + sizeof(Entity2);
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
	
	char* ChunkList::GetComponentData(
		Chunk* chunk,
		const ArchetypeLayout& layout,
		size_t index)
	{
		return GetChunkStream(chunk, layout)
			+ layout.Component.lock()->Size
			* index;
	}
}
