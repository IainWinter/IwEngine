#include "iw/entity/ChunkList.h"
#include <assert.h>
#include "iw/log/logger.h"

namespace IW {
	using iterator = ChunkList::iterator;

	// replace with free list like thing but for valid entities 
	iterator& iterator::operator++() {
		do {
			m_index++;
			if (m_index == m_chunk->EndIndex()) {
				do {
					m_chunk = m_chunk->Next;
					if (m_chunk) {
						m_index = m_chunk->BeginIndex();
					}
				} while (m_chunk && m_chunk->Count == 0);
			}
		} while (m_chunk && !m_chunk->GetEntity(m_index)->Alive);

		return *this;
	}

	bool iterator::operator==(
		const iterator& itr) const
	{
		return this->m_chunk == itr.m_chunk
			&& this->m_index == itr.m_index;
	}

	bool iterator::operator!=(
		const iterator& itr) const
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		for (size_t i = 0; i < m_indices->Count; i++) {
			m_data->Components[i] = m_chunk->GetComponentData(
				m_archetype->Layout[m_indices->Indices[i]], m_index);
		}

		Entity* entity = m_chunk->GetEntity(m_index);

		if (   m_chunk 
			&& m_index == m_chunk->EndIndex())
		{
			LOG_INFO << "dddd";
		}

		if (entity->Index > 98734259873) {
			LOG_INFO << "dddd";
		}

		return EntityComponentData { entity->Index, entity->Version, *m_data };
	}

	iterator::iterator(
		Chunk* chunk,
		size_t index,
		const iw::ref<Archetype>& archetype,
		const iw::ref<ComponentQuery>& query)
		: m_chunk(chunk)
		, m_index(index)
		, m_archetype(archetype)
	{
		size_t cdSize = sizeof(ComponentData)
			+ sizeof(size_t)
			* query->Count;

		ComponentData* cd = (ComponentData*)malloc(cdSize);
		assert(cd);
		memset(cd, 0, cdSize);

		m_data = iw::ref<ComponentData>(cd, free);

		size_t cdisSize = sizeof(ComponentDataIndices)
			+ sizeof(size_t)
			* query->Count;

		ComponentDataIndices* cdis = (ComponentDataIndices*)malloc(cdisSize);
		assert(cdis);
		memset(cdis, 0, cdisSize);

		cdis->Count = query->Count;
		for (size_t i = 0; i < query->Count; i++) {
			for (size_t j = 0; j < archetype->Count; j++) {
				if (query->Components[i]->Type == archetype->Layout[j].Component->Type) {
					cdis->Indices[i] = j;
					break;
				}
			}
		}

		m_indices = iw::ref<ComponentDataIndices>(cdis, free);
	}

	ChunkList::ChunkList(
		const iw::ref<Archetype>& archetype,
		size_t chunkSize)
		: m_root(nullptr)
		, m_count(0)
		, m_chunkCount(0)
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

		++m_count;

		return index;
	}

	bool ChunkList::ReinstateComponents(
		const iw::ref<EntityData>& entityData)
	{
		Chunk* chunk = FindChunk(entityData->ChunkIndex);
		if (chunk) {
			chunk->ReinstateComponents();

			Entity* entityComponent = chunk->GetEntity(entityData->ChunkIndex);
			*entityComponent = entityData->Entity;

			++m_count;

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

			--m_count;

			// If chunk is empty free it
			if (chunk->Count == 0) {			
				if (chunk->Previous) {
					chunk->Previous->Next = chunk->Next;
				}

				if (chunk->Next) {
					chunk->Next->Previous = chunk->Previous;
				}

				if (chunk == m_root) {
					m_root = m_root->Next;
				}

				LOG_DEBUG << "Deleting Chunk " << chunk->IndexOffset / chunk->Capacity;

				--m_chunkCount;
				free(chunk);
			}

			return true;
		}

		return false;
	}

	void* ChunkList::GetComponentData(
		const iw::ref<Component>& component,
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

			if (i == m_archetype->Count) {
				return nullptr;
			}

			return chunk->GetComponentData(m_archetype->Layout[i], index);
		}

		return nullptr;
	}

	iterator ChunkList::Begin(
		const iw::ref<ComponentQuery>& query)
	{
		Chunk* chunk = m_root;
		size_t index = 0;
		while (chunk && chunk->Count == 0) {
			chunk = chunk->Next;
		}
		
		if (chunk) {
			index = chunk->BeginIndex();
		}

		return iterator(m_root, index, m_archetype, query);
	}

	iterator ChunkList::End(
		const iw::ref<ComponentQuery>& query)
	{
		Chunk* chunk = m_root;
		size_t index = 0;
		while (chunk && chunk->Next) {
			chunk = chunk->Next;
		}

		if (chunk) {
			index = chunk->EndIndex();
		}

		return iterator(nullptr, index, m_archetype, query);
	}

	Chunk* ChunkList::FindChunk(
		size_t index)
	{
		Chunk* chunk = m_root;
		while (chunk && !chunk->ContainsIndex(index)) {
			chunk = chunk->Next;
		}

		return chunk;
	}

	Chunk* ChunkList::CreateChunk() {
		Chunk* chunk = (Chunk*)malloc(m_chunkSize);
		assert(chunk);
		memset(chunk, 0, m_chunkSize);

		chunk->IndexOffset = m_chunkCapacity * m_chunkCount;
		chunk->Capacity    = m_chunkCapacity;

		LOG_DEBUG << "Creating Chunk " << chunk->IndexOffset / chunk->Capacity;

		++m_chunkCount;

		return chunk;
	}

	Chunk& ChunkList::FindOrCreateChunk() {
		if (!m_root) {
			m_root = CreateChunk();
		}

		Chunk* chunk = m_root;
		while (chunk && chunk->CurrentIndex == chunk->Capacity) {
			if (chunk->Next == nullptr) {
				chunk->Next = CreateChunk();
				chunk->Next->Previous = chunk;
			}

			chunk = chunk->Next;
		}

		return *chunk;
	}

	size_t ChunkList::GetChunkCapacity(
		const iw::ref<Archetype>& archetype)
	{
		size_t archetypeSize = archetype->Size + sizeof(Entity);
		size_t bufSize       = m_chunkSize - sizeof(Chunk);
		size_t padSize       = bufSize % archetypeSize;

		return (bufSize - padSize) / archetypeSize;
	}
}
