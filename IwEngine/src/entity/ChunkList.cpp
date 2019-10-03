#include "iw/entity/ChunkList.h"
#include <assert.h>
#include "iw/log/logger.h"

namespace IwEntity {
	using iterator = ChunkList::iterator;

	iterator& iterator::operator++() {
		if (m_chunk->IsLast(m_index)) {
			m_chunk = m_chunk->Next;
		}

		m_index++;
		if (m_chunk) {
			while (!m_chunk->IsLast(m_index + 1)
				&& !m_chunk->GetEntity(m_index)->Alive) // this might be wrong?
			{
				m_index++;
			}
		}

		// replace with free list like thing but for valid entities 
		
		return *this;
	}

	bool iterator::operator==(
		const iterator& itr)
	{
		return this->m_chunk == itr.m_chunk
			&& this->m_index == itr.m_index;
	}

	bool iterator::operator!=(
		const iterator& itr)
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		for (size_t i = 0; i < m_indices->Count; i++) {
			m_data->Components[i] = m_chunk->GetComponentData(
				m_archetype->Layout[m_indices->Indices[i]], m_index);
		}

		Entity* entity = m_chunk->GetEntity(m_index);
		return EntityComponentData { entity->Index, entity->Version, *m_data };
	}

	iterator::iterator(
		Chunk* chunk,
		size_t index,
		const iwu::ref<Archetype>& archetype,
		const iwu::ref<ComponentQuery>& query)
		: m_chunk(chunk)
		, m_index(index)
		, m_archetype(archetype)
	{
		size_t bufSize = sizeof(ComponentData)
			+ sizeof(size_t)
			* query->Count;

		ComponentData* buf = (ComponentData*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		m_data = iwu::ref<ComponentData>(buf, free);

		size_t bufSize1 = sizeof(ComponentDataIndices)
			+ sizeof(size_t)
			* query->Count;

		ComponentDataIndices* buf1 = (ComponentDataIndices*)malloc(bufSize);
		assert(buf1);
		memset(buf1, 0, bufSize1);

		buf1->Count = query->Count;
		for (size_t i = 0; i < query->Count; i++) {
			for (size_t j = 0; j < archetype->Count; j++) {
				if (query->Components[i]->Type == archetype->Layout[j].Component->Type) {
					buf1->Indices[i] = j;
					break;
				}
			}
		}

		m_indices = iwu::ref<ComponentDataIndices>(buf1, free);
	}

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

			Entity* entityComponent = chunk->GetEntity(entityData->ChunkIndex);
			*entityComponent = entityData->Entity;

			m_count++;

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

			m_count--;

			//If chunk is empty free it
			if (chunk->Count == 0) {
				if (chunk->Previous) {
					chunk->Previous->Next = chunk->Next;
				}

				else if (chunk->Next) {
					chunk->Next->Previous = chunk->Previous;
				}

				else {
					m_current = nullptr; // This needs to be looked at, maybe 
					m_chunks.clear();    //  might need to delete sooner
					m_chunks.shrink_to_fit();
					m_count = 0;
				}

				free(chunk);
			}


			return true;
		}

		return false;
	}

	void* ChunkList::GetComponentData(
		const iwu::ref<Component>& component,
		size_t index)
	{
		Chunk* chunk = FindChunk(index);
		if (chunk) {
			size_t i = 0;
			for (; i < m_archetype->Count; i++) {
				if (component->Type == m_archetype->Layout[i].Component->Type) {
					break;
				}
			}

			return chunk->GetComponentData(m_archetype->Layout[i], index);
		}

		return nullptr;
	}

	iterator ChunkList::begin(
		const iwu::ref<ComponentQuery>& query)
	{
		if (m_chunks.size() == 0) { // todo: Make this code less ugly
			return iterator(nullptr, 0, m_archetype, query);
		}

		return iterator(m_chunks.front(), 0, m_archetype, query);
	}

	iterator ChunkList::end(
		const iwu::ref<ComponentQuery>& query)
	{
		if (m_chunks.size() == 0) {
			return iterator(nullptr, 0, m_archetype, query);
		}

		return iterator(nullptr, m_chunks.back()->LastIndex() + 1, m_archetype, query);
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
		chunk->Capacity    = m_chunkCapacity;

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
		return chunk->GetComponentStream(layout)
			+ layout.Component->Size
			* (index - chunk->EntityIndex);
	}
}
